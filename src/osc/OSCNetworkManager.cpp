#include "OSCNetworkManager.h"

#include <QTime>
#include <QUuid>

// http://www.rfc-editor.org/rfc/rfc1055.txt
#define SLIP_END		0xc0    /* indicates end of packet */
#define SLIP_ESC		0xdb    /* indicates byte stuffing */
#define SLIP_ESC_END	0xdc    /* ESC ESC_END means END data byte */
#define SLIP_ESC_ESC	0xdd    /* ESC ESC_ESC means ESC data byte */

#define SLIP_CHAR(x)	static_cast<char>(static_cast<unsigned char>(x))


OSCNetworkManager::OSCNetworkManager(QObject* parent, QStringList availableTypes)
    : QObject(parent)
    , m_availableTypes(availableTypes)
    , m_ipAddress(QHostAddress::LocalHost)
    , m_udpTxPort(DEFAULT_UDP_TX_PORT)
    , m_tcpPort(DEFAULT_TCP_PORT)
    , m_isEnabled(true)
	, m_useTcp(true)
	, m_tcpFrameMode(OSCStream::FRAME_MODE_1_0)
	, m_log()
	, m_logIncomingMsg(true)
    , m_logOutgoingMsg(true)
	, m_incompleteStreamData()
{
    // prepare log changed signal:
    m_logChangedSignalDelay.setSingleShot(true);
    m_logChangedSignalDelay.setInterval(100);
    connect(&m_logChangedSignalDelay, SIGNAL(timeout()), this, SIGNAL(logChanged()));

	// prepare timer that is used to try to connect again to TCP target:
	m_tryConnectAgainTimer.setSingleShot(true);
	connect(&m_tryConnectAgainTimer, SIGNAL(timeout()), this, SLOT(tryToConnectTCP()));

	// connect TCP socket with onConnected and onError slots:
	connect(&m_tcpSocket, SIGNAL(connected()), this, SLOT(onConnected()));
	connect(&m_tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError()));
    connect(&m_udpSocket, &QUdpSocket::stateChanged, this, &OSCNetworkManager::isConnectedChanged);
	connect(&m_udpSocket, SIGNAL(readyRead()), this, SLOT(readIncomingUdpDatagrams()));
    connect(&m_tcpSocket, SIGNAL(readyRead()), this, SLOT(readIncomingTcpStream()));
}

void OSCNetworkManager::createAndLoadPreset(QString type, QString name, QString protocol,
                                            QString ip, int udpTxPort,
                                            int udpRxPort, int tcpPort, QString originalPreset) {
    QString id = createPreset(type, name, protocol, ip, udpTxPort, udpRxPort, tcpPort);
    loadPreset(id);
    if (!originalPreset.isEmpty()) {
        removePreset(originalPreset);
    }
}

QString OSCNetworkManager::createPreset(QString type, QString name, QString protocol,
                                        QString ip, int udpTxPort,
                                        int udpRxPort, int tcpPort) {
    QJsonObject preset;
    preset["type"] = type;
    preset["name"] = name;
    preset["protocol"] = protocol;
    preset["ip"] = ip;
    preset["udpTxPort"] = udpTxPort;
    preset["udpRxPort"] = udpRxPort;
    preset["tcpPort"] = tcpPort;
    QString id = QUuid::createUuid().toString();
    m_presets.insert(id, preset);
    m_presetIds.append(id);
    emit presetsChanged();
    return id;
}

void OSCNetworkManager::loadPreset(QString presetId) {
    if (presetId == m_currentPresetId) return;
    if (presetId.isEmpty()) {
        // empty ID -> disable OSC connection:
        closeConnection();
        m_currentPresetId = presetId;
        emit currentPresetChanged();
        return;
    }

    if (!m_presetIds.contains(presetId)) {
        qWarning() << "OSCNetworkManager: tried to load unkown preset:" << presetId;
        return;
    }
    QJsonObject preset = getPreset(presetId);
    if (!m_availableTypes.contains(preset["type"].toString())) {
        qWarning() << "OSCNetworkManager: tried to load preset of wrong type:" << preset["type"].toString();
        return;
    }

    m_currentPresetId = presetId;
    m_currentConnectionType = preset["type"].toString();
    m_ipAddress = QHostAddress(preset["ip"].toString());
    m_udpTxPort = quint16(preset["udpTxPort"].toInt());
    m_tcpPort = quint16(preset["tcpPort"].toInt());
    emit currentPresetChanged();
    reconnect();
}

QJsonObject OSCNetworkManager::getPreset(QString presetId) const {
    return m_presets.value(presetId, QJsonObject());
}

void OSCNetworkManager::removePreset(QString presetId) {
    if (!m_presetIds.contains(presetId)) return;
    if (m_currentPresetId == presetId) {
        if (m_presetIds.size() > 1) {
            for (QString otherId: m_presetIds) {
                if (otherId != presetId) {
                    loadPreset(otherId);
                    break;
                }
            }
        } else {
            loadPreset("");
        }
    }

    m_presetIds.removeAll(presetId);
    m_presets.remove(presetId);
    emit presetsChanged();
}

void OSCNetworkManager::closeConnection() {
    m_tryConnectAgainTimer.stop();
    if (m_tcpSocket.state() != QAbstractSocket::UnconnectedState) {
        m_tcpSocket.abort();
    }
    m_udpSocket.close();
    emit isConnectedChanged();
}

void OSCNetworkManager::reconnect() {
    QJsonObject preset = getCurrentPreset();

    closeConnection();

    // start new connection:
    if (preset["protocol"].toString() == OscProtocol::UDP) {
        m_useTcp = false;
        m_udpSocket.bind(quint16(preset["udpRxPort"].toInt()));
        emit isConnectedChanged();
    } else if (preset["protocol"].toString() == OscProtocol::TCP_1_0) {
        m_useTcp = true;
        m_tcpFrameMode = OSCStream::FRAME_MODE_1_0;
        m_tryConnectAgainTimer.start(20);
    } else if (preset["protocol"].toString() == OscProtocol::TCP_1_1) {
        m_useTcp = true;
        m_tcpFrameMode = OSCStream::FRAME_MODE_1_1;
        m_tryConnectAgainTimer.start(20);
    } else {
        qWarning() << "OSCNetworkManager: preset has invalid protocol";
    }
}

void OSCNetworkManager::setEnabled(bool value) {
    m_isEnabled = value;
    emit enabledChanged();
    emit isConnectedChanged();
}

void OSCNetworkManager::sendMessage(QString messageString, bool forced)
{
    if (!m_isEnabled && !forced) return;

    size_t outSize = 0;
	char* packet = OSCPacketWriter::CreateForString(messageString.toLatin1().data(), outSize);
	sendMessageData(packet, outSize);

	// Log if logging of outgoing messages is enabled:
    addToLog(true, messageString);
}

void OSCNetworkManager::sendMessage(QString path, QString argument, bool forced)
{
    if (!m_isEnabled && !forced) return;

    size_t outSize = 0;
	OSCPacketWriter packetWriter(path.toStdString());
	packetWriter.AddString(argument.toStdString());
	char* packet = packetWriter.Create(outSize);
	sendMessageData(packet, outSize);

	// Log if logging of outgoing messages is enabled:
    addToLog(true, path + "=" + argument);
}

void OSCNetworkManager::sendMessage(QString path, double argument, bool forced)
{
    if (!m_isEnabled && !forced) return;

    size_t outSize = 0;
    OSCPacketWriter packetWriter(path.toStdString());
    packetWriter.AddFloat64(argument);
    char* packet = packetWriter.Create(outSize);
    sendMessageData(packet, outSize);

    // Log if logging of outgoing messages is enabled:
    addToLog(true, path + "=" + QString::number(argument));
}

void OSCNetworkManager::sendMessage32bit(QString path, float argument, bool forced)
{
    if (!m_isEnabled && !forced) return;

    size_t outSize = 0;
    OSCPacketWriter packetWriter(path.toStdString());
    packetWriter.AddFloat32(argument);
    char* packet = packetWriter.Create(outSize);
    sendMessageData(packet, outSize);

    // Log if logging of outgoing messages is enabled:
    addToLog(true, path + "=" + QString::number(double(argument)));
}

void OSCNetworkManager::sendMessage(QString path, qreal argument1, qreal argument2, bool forced)
{
    if (!m_isEnabled && !forced) return;

    size_t outSize = 0;
    OSCPacketWriter packetWriter(path.toStdString());
    packetWriter.AddFloat64(argument1);
    packetWriter.AddFloat64(argument2);
    char* packet = packetWriter.Create(outSize);
    sendMessageData(packet, outSize);

    // Log if logging of outgoing messages is enabled:
    addToLog(true, path + "=" + QString::number(argument1) + "," + QString::number(argument2));
}

void OSCNetworkManager::sendMessage(QString path, QString argument1, QString argument2, bool forced)
{
    if (!m_isEnabled && !forced) return;

    size_t outSize;
    OSCPacketWriter packetWriter(path.toStdString());
    packetWriter.AddString(argument1.toStdString());
    packetWriter.AddString(argument2.toStdString());
    char* packet = packetWriter.Create(outSize);
    sendMessageData(packet, outSize);

    // Log if logging of outgoing messages is enabled:
    addToLog(true, path + "=" + argument1 + "," + argument2);
}

QStringList OSCNetworkManager::getProtocolNames() const
{
    return QStringList {OscProtocol::UDP, OscProtocol::TCP_1_0, OscProtocol::TCP_1_1};
}

QJsonObject OSCNetworkManager::getState() const {
	QJsonObject state;
    state["valid"] = true;
    state["currentPresetId"] = getCurrentPresetId();
    state["presets"] = serialize<QMap<QString, QJsonObject>>(m_presets);
    state["presetIds"] = serialize<QStringList>(m_presetIds);
    state["logIncoming"] = getLogIncomingIsEnabled();
	state["logOutgoing"] = getLogOutgoingIsEnabled();
	return state;
}

void OSCNetworkManager::setState(const QJsonObject &state) {
	if (!state["valid"].toBool()) return;
    enableLogging(state["logIncoming"].toBool(), state["logOutgoing"].toBool());

    m_presets = deserialize<QMap<QString, QJsonObject>>(state["presets"].toString());
    m_presetIds = deserialize<QStringList>(state["presetIds"].toString());
    emit presetsChanged();

    QString presetId = state["currentPresetId"].toString();
    if (!presetId.isEmpty()) {
        loadPreset(presetId);
    } else if (!state["ipAddress"].toString().isEmpty()) {
        createAndLoadPreset(m_availableTypes.first(), "",
                state["protocol"].toString(),
                state["ipAddress"].toString(),
                state["udpTxPort"].toInt(),
                state["udpRxPort"].toInt(),
                state["tcpPort"].toInt());
    }
}

void OSCNetworkManager::sendMessageData(char* packet, size_t outSize)
{
    // send packet either with UDP or TCP:
    if (m_useTcp) {
        // check if TCP socket is connected:
        if (m_tcpSocket.state() == QAbstractSocket::ConnectedState) {
            // socket is connected
            // for TCP transmission the packet has to be framed:
            char* framedPacket = OSCStream::CreateFrame(m_tcpFrameMode, packet, outSize);
            m_tcpSocket.write(framedPacket, qint64(outSize));
            delete[] framedPacket;
        }
    } else {
        // use UDP:
        m_udpSocket.writeDatagram(packet, qint64(outSize), m_ipAddress, m_udpTxPort);
    }

    delete[] packet;
    emit packetSent();
}

QByteArray OSCNetworkManager::popPacketFromStreamData(QByteArray& data) const
{
	if (m_tcpFrameMode == OSCStream::FRAME_MODE_1_0) {
		return popPacketLengthFramedPacketFromStreamData(data);
	} else {
		return popSlipFramedPacketFromStreamData(data);
	}
}

QByteArray OSCNetworkManager::popPacketLengthFramedPacketFromStreamData(QByteArray& tcpData) const
{
	// the first 4 bytes are the length of the OSC message following as an int32:
	int32_t messageLength = reinterpret_cast<int32_t*>(tcpData.data())[0];
	// adjust the network byte order (?):
	OSCArgument::Swap32(&messageLength);

	// check if the messageLength is in the range of a valid message:
	if (messageLength <= 0 || messageLength > 512) {
		// this is not a valid OSC message
		// received data will be discarded:
		addToLog(false, "Invalid data received (message length in TCP packet is out of range). Check Protocol Settings.");
		tcpData.resize(0);
		return QByteArray();
	}

	// check if the message is completely received:
	if (messageLength > tcpData.size()) {
		// the message length is greater than the size of the data received
		// so there is nothing to pop / return yet:
		return QByteArray();
	}

	// create a new QByteArray containing only the OSC message data:
	QByteArray messageData = QByteArray(tcpData.data() + sizeof(messageLength), messageLength);
	// remove the messageLength and the message data from the received data:
    tcpData.remove(0, messageLength + int(sizeof(messageLength)));

	return messageData;
}

QByteArray OSCNetworkManager::popSlipFramedPacketFromStreamData(QByteArray& tcpData) const
{
	// A SLIP framed packet begins and ends with a SLIP END character.

	// find first SLIP END character:
	// most probably it is the first character:
	int firstSlipEndPosition = 0;
	if (tcpData[0] != SLIP_CHAR(SLIP_END)) {
		for (int i=1; i<tcpData.size(); ++i) {
			if (tcpData[i] == SLIP_CHAR(SLIP_END)) {
				firstSlipEndPosition = i;
				break;
			}
		}
		if (firstSlipEndPosition == 0) {
			// if first slip end postion is still 0,
			// this means there is no SLIP END character in the data
			// -> discard the data and return nothing:
			addToLog(false, "[In] Invalid data received (missing SLIP END character). Check Protocol Settings.");
			tcpData.resize(0);
			return QByteArray();
		}
	}

	// find second SLIP END character:
	int secondSlipEndPosition = -1;
	for (int i=firstSlipEndPosition+1; i<tcpData.size(); ++i) {
		if (tcpData[i] == SLIP_CHAR(SLIP_END)) {
			secondSlipEndPosition = i;
			break;
		}
	}
	if (secondSlipEndPosition < 0) {
		// there is no second SLIP END character
		// -> message is not yet complete:
		return QByteArray();
	}

	// at this point we should have found two SLIP END characters
	Q_ASSERT(secondSlipEndPosition > firstSlipEndPosition);

	int messageLength = secondSlipEndPosition - firstSlipEndPosition - 1;
	// create a new QByteArray containing only the OSC message data:
	QByteArray messageData = QByteArray(tcpData.data() + firstSlipEndPosition + 1, messageLength);
	// remove everything before the second SLIP END from the received data:
	tcpData.remove(0, secondSlipEndPosition + 1);

	// replace characters that have been escaped because of SLIP framing:
	messageData.replace(SLIP_CHAR(SLIP_ESC) + SLIP_CHAR(SLIP_ESC_END), SLIP_CHAR(SLIP_END));
	messageData.replace(SLIP_CHAR(SLIP_ESC) + SLIP_CHAR(SLIP_ESC_ESC), SLIP_CHAR(SLIP_ESC));

	return messageData;
}

void OSCNetworkManager::addToLog(bool out, QString text) const
{
	if (out && m_logOutgoingMsg) {
		QString time = "[" + QTime::currentTime().toString() + "] [Out] ";
		m_log.prepend(time + text);
		if (m_log.size() > MAX_LOG_LENGTH) m_log.removeLast();
        if (!m_logChangedSignalDelay.isActive()) m_logChangedSignalDelay.start();
	} else if (!out && m_logIncomingMsg) {
		QString time = "[" + QTime::currentTime().toString() + "] [In]  ";
		m_log.prepend(time + text);
		if (m_log.size() > MAX_LOG_LENGTH) m_log.removeLast();
        if (!m_logChangedSignalDelay.isActive()) m_logChangedSignalDelay.start();
	}
}

void OSCNetworkManager::tryToConnectTCP()
{
    if (!m_useTcp) return;
    if (m_tcpSocket.state() != QAbstractSocket::UnconnectedState) return;
	m_tcpSocket.connectToHost(m_ipAddress, m_tcpPort);
}

void OSCNetworkManager::onConnected()
{
	emit isConnectedChanged();
}

void OSCNetworkManager::onError()
{
	emit isConnectedChanged();
	addToLog(true, "TCP Error: " + m_tcpSocket.errorString());

	// try again if user still wants to use TCP:
	if (m_useTcp) {
		m_tryConnectAgainTimer.start(TRY_CONNECT_AGAIN_TIME);
	}
}

bool OSCNetworkManager::isConnected() const
{
	if (m_useTcp) {
		return m_tcpSocket.state() == QAbstractSocket::ConnectedState;
    } else {
        return m_udpSocket.state() ==  QAbstractSocket::BoundState;
    }
}

void OSCNetworkManager::enableLogging(bool incoming, bool outgoing)
{
	m_logIncomingMsg = incoming; m_logOutgoingMsg = outgoing;
}

void OSCNetworkManager::readIncomingUdpDatagrams()
{
	while (m_udpSocket.hasPendingDatagrams()) {
		// prepare empty variables to be written in:
		QByteArray datagram;
        datagram.resize(int(m_udpSocket.pendingDatagramSize()));
		QHostAddress sender;
		quint16 senderPort;

		// get data from socket:
		m_udpSocket.readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

		// process data:
		processIncomingRawData(datagram);
	}
}

void OSCNetworkManager::readIncomingTcpStream()
{
    const QByteArray newStreamData = m_tcpSocket.readAll();
    QByteArray streamData = newStreamData;

	// check if there is incomplete stream data left from last call:
    bool usingIncompleteStreamData = false;
    if (!m_incompleteStreamData.isEmpty()) {
        streamData = m_incompleteStreamData + newStreamData;
        m_incompleteStreamData.clear();
        usingIncompleteStreamData = true;
    }

    int packetCount = 0;

	// as long as there is stream data left:
	while (streamData.size()) {
		// try to get a complete packet from the stream data:
		QByteArray packet = popPacketFromStreamData(streamData);

        if (packet.isEmpty()) {

            if (packetCount < 1 && usingIncompleteStreamData) {
                // couldn't even get one package,
                // try parsing only new data:
                QByteArray newData = newStreamData;
                QByteArray packet = popPacketFromStreamData(newData);
                if (!packet.isEmpty()) {
                    // successfully got packet from only new data,
                    // there seems to be a problem with the incomplete data:
                    m_incompleteStreamData.clear();
                    streamData = newStreamData;
                    usingIncompleteStreamData = false;
                    continue;
                }
            }

			// there is no more complete packet
			// but maybe there is incomplete packet data left:
			m_incompleteStreamData = streamData;
			return;
		}

		processIncomingRawData(packet);
        ++packetCount;
	}
}

void OSCNetworkManager::processIncomingRawData(QByteArray msgData)
{
	// check if the data is a single message or a bundle of messages:
	if (msgData[0] == '/') {
		// it starts with a "/" -> it is a single message:
		processIncomingRawMessage(msgData);
	} else if (msgData.startsWith("#bundle")) {
		// it starts with "#bundle" -> it is a bundle
		// remove "#bundle" string (8 bytes) and unused timetag (8 bytes):
		msgData.remove(0, 16);
		// try to get all messages in the bundle:
		while (msgData.size()) {
			// each message starts with the length of the message as int32
			// this is the same as a TCP 1.0 packet, so the function for that can be used:
			QByteArray message = popPacketLengthFramedPacketFromStreamData(msgData);
			// process the message:
            processIncomingRawData(message);
		}
	} else {
		// invalid data
		addToLog(false, "[Invalid] Raw: " + QString::fromLatin1(msgData.data(), msgData.size()));
	}
}

void OSCNetworkManager::processIncomingRawMessage(QByteArray msgData)
{
	// build an OSC message from the data:
	OSCMessage msg(msgData);

	// Log if logging of incoming messages is enabled:
	if (msg.isValid()) {
		addToLog(false, msg.pathString() + msg.getArgumentsAsDebugString());
	} else {
		addToLog(false, "[Invalid] Raw: " + QString::fromLatin1(msgData.data(), msgData.size()));
	}

	// emit message received signal:
	if (msg.isValid()) {
        if (m_isEnabled) {
            emit messageReceived(msg);
        } else {
            emit messageReceivedWhileDisabled(msg);
        }
	}
}
