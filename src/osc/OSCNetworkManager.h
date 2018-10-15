#ifndef OSCNETWORKMANAGER_H
#define OSCNETWORKMANAGER_H

#include "OSCParser.h"
#include "OSCMessage.h"
#include "utils.h"

#include <QObject>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QTimer>
#include <QJsonObject>


/**
 * @brief time to try to connect again after an error in ms
 * @memberof OSCNetworkManager
 */
static const int TRY_CONNECT_AGAIN_TIME = 3000;  // ms

/**
 * @brief default TCP port
 * @memberof OSCNetworkManager
 */
static const quint16 DEFAULT_TCP_PORT = 3032;

/**
 * @brief default UDP Tx port
 * @memberof OSCNetworkManager
 */
static const quint16 DEFAULT_UDP_TX_PORT = 8001;

/**
 * @brief default UDP Rx port
 * @memberof OSCNetworkManager
 */
static const quint16 DEFAULT_UDP_RX_PORT = 8000;

/**
 * @brief maximum entries in OSC log
 * @memberof OSCNetworkManager
 */
static const int MAX_LOG_LENGTH = 1000;

namespace OscProtocol {
static const QString UDP = "UDP";
static const QString TCP_1_0 = "TCP 1.0";
static const QString TCP_1_1 = "TCP 1.1";
}

namespace OscConnectionType {
static const QString Eos = "Eos";
static const QString Hog4 = "Hog4";
static const QString X32 = "X32";
static const QString X_Air = "X Air";
static const QString Custom = "Custom";
}


/**
 * @brief The OSCNetworkManager class manages OSC data exchange.
 * It can send and receive OSC messages via UDP and TCP
 * and supports OSC 1.0 and 1.1 packet-framing.
 */
class OSCNetworkManager : public QObject {

	Q_OBJECT

    Q_PROPERTY(bool enabled READ getEnabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(QString currentPresetId READ getCurrentPresetId NOTIFY currentPresetChanged)
    Q_PROPERTY(QJsonObject currentPreset READ getCurrentPreset NOTIFY currentPresetChanged)
    Q_PROPERTY(QStringList presetIds READ getPresetIds NOTIFY presetsChanged)
    Q_PROPERTY(bool connected READ isConnected NOTIFY isConnectedChanged)

public:
    /**
     * @brief OSCNetworkManager creates an OSCNetworkManager object
     */
    explicit OSCNetworkManager(QObject* parent, QStringList availableTypes);

    // declare EosOSCManager as friend to be able to call for example reconnect():
    friend class EosOSCManager;


public slots:

    // --------------------- Presets ------------------------

    QStringList getAvailableTypes() const { return m_availableTypes; }

    void createAndLoadPreset(QString type, QString name, QString protocol,
                      QString ip, int udpTxPort, int udpRxPort, int tcpPort, QString originalPreset = "");

    QString createPreset(QString type, QString name, QString protocol,
                      QString ip, int udpTxPort, int udpRxPort, int tcpPort);

    void loadPreset(QString presetId);

    QString getCurrentPresetId() const { return m_currentPresetId; }

    QJsonObject getCurrentPreset() const { return getPreset(getCurrentPresetId()); }

    QString getCurrentType() const { return m_currentConnectionType; }

    QStringList getPresetIds() const { return m_presetIds; }

    QJsonObject getPreset(QString presetId) const;

    void removePreset(QString presetId);

    void closeConnection();

    void reconnect();

    // ------------------- Getter / Setter --------------------

	/**
	 * @brief getEnabled returns if OSC output is enabled
	 * @return true if OSC trigger output is enabled
	 */
	bool getEnabled() const { return m_isEnabled; }
	/**
	 * @brief setEnabled sets if OSC output is enabled
	 * @param value true to enable
	 */
    void setEnabled(bool value);

	/**
	 * @brief isConnected returns if the TCP socket is connected, returns true if UDP is used
	 * @return true if in "connected" state
	 */
    bool isConnected() const;

    /**
     * @brief getProtocolNames returns a list of all available protocols
     * @return a list of strings
     */
    QStringList getProtocolNames() const;

	// ------------------- Logging --------------------

	/**
	 * @brief getLog returns the log as a QStringList to be displayed in UI
	 * @return log as QStringList
	 */
	QStringList getLog() const { return m_log; }

	/**
	 * @brief enableLogging enables logging separatly for incoming and outgoing messages
	 * @param incoming true to enable logging of incoming messages
	 * @param outgoing true to enable logging of outgoing messages
	 */
	void enableLogging(bool incoming, bool outgoing);

	/**
	 * @brief getLogIncomingIsEnabled returns if logging of incoming messages is enabled
	 * @return true if enabled
	 */
	bool getLogIncomingIsEnabled() const { return m_logIncomingMsg; }

	/**
	 * @brief getLogOutgoingIsEnabled returns if logging of outgoing messages is enabled
	 * @return true if enabled
	 */
	bool getLogOutgoingIsEnabled() const { return m_logOutgoingMsg; }

	/**
	 * @brief clearLog clears the log
	 */
	void clearLog() const { m_log.clear(); emit logChanged(); }

	// ------------------- Send Message --------------------

	/**
	 * @brief Sends an OSC packet
	 * @param messageString path and arguments as a string in the format: /x/y/z=1.0,2.0
	 * @param forced true to send message even if OSC output is disabled
	 */
	void sendMessage(QString messageString, bool forced = false);

	/**
	 * @brief Sends an OSC message with a string as the only argument
	 * @param path of the message
	 * @param argument a string to be sent as the only argument
	 * @param forced true to send message even if OSC output is disabled
	 */
	void sendMessage(QString path, QString argument, bool forced = false);

    /**
     * @brief Sends an OSC message with a 64-bit double as the only argument
     * @param path of the message
     * @param argument a double to be sent as the only argument
     * @param forced true to send message even if OSC output is disabled
     */
    void sendMessage(QString path, double argument, bool forced = false);

    /**
     * @brief Sends an OSC message with a 32-bit float as the only argument
     * @param path of the message
     * @param argument a float to be sent as the only argument
     * @param forced true to send message even if OSC output is disabled
     */
    void sendMessage32bit(QString path, float argument, bool forced = false);

    /**
     * @brief Sends an OSC message with two floats as the arguments
     * @param path of the message
     * @param argument1 a float to be sent as the first argument
     * @param argument2 a float to be sent as the second argument
     * @param forced true to send message even if OSC output is disabled
     */
    void sendMessage(QString path, qreal argument1, qreal argument2, bool forced = false);

    /**
     * @brief Sends an OSC message with two strings as the arguments
     * @param path of the message
     * @param argument1 a string to be sent as the first argument
     * @param argument2 a string to be sent as the second argument
     * @param forced true to send message even if OSC output is disabled
     */
    void sendMessage(QString path, QString argument1, QString argument2, bool forced = false);


	// ------------------- Persistence --------------------

	/**
	 * @brief getState returns the settings of this manager to persist them
	 * @return the settings as a QJsonObject
	 */
	QJsonObject getState() const;

	/**
	 * @brief setState restores the settings from a saved Json object
	 * @param state a QJsonObject previously generated by getState()
	 */
    void setState(const QJsonObject& state);

signals:

	// ------------------- Receive Message --------------------

	/**
	 * @brief messageReceived emitted when an OSC messages has been received
	 * @param msg the received message
	 */
	void messageReceived(OSCMessage msg);

    void messageReceivedWhileDisabled(OSCMessage msg);

	// ------------------- Status --------------------

    void currentPresetChanged();

    void presetsChanged();

    void enabledChanged();

	/**
	 * @brief will be emitted when a packet is successfully sent
	 */
    void packetSent();

	/**
	 * @brief will be emitted when the connection state changed
	 */
    void isConnectedChanged();

	/**
	 * @brief will be emitted when the log has changed
	 */
	void logChanged() const;


protected:

    // ------------------- Private / Internal --------------------

	/**
	 * @brief sendMessageData sends raw OSC message data
	 * @param packet OSC message as an array of chars
	 * @param outSize size of the packet
	 */
	void sendMessageData(char* packet, size_t outSize);

	/**
	 * @brief popPacketFromStreamData returns and removes the
	 * raw OSC message data from a framed packet in a TCP stream
	 * or returns nothing if the OSC message is not yet complete.
	 * Discards the data if it is invalid.
	 * @param data the incoming stream data (will be modified)
	 * @return the data of a single OSC message
	 */
	QByteArray popPacketFromStreamData(QByteArray& data) const;
	/**
	 * @brief returns and removes the raw OSC message data from
	 * a "packet-length-framed" packet in a TCP stream
	 * or returns nothing if the OSC message is not yet complete.
	 * Discards the data if it is invalid.
	 * @param data the incoming stream data (will be modified)
	 * @return the data of a single OSC packet (could be a bundle or message)
	 */
	QByteArray popPacketLengthFramedPacketFromStreamData(QByteArray& data) const;
	/**
	 * @brief returns and removes the raw OSC message data from
	 * a SLIP framed packet in a TCP stream
	 * or returns nothing if the OSC message is not yet complete.
	 * Discards the data if it is invalid.
	 * @param data the incoming stream data (will be modified)
	 * @return the data of a single OSC packet (could be a bundle or message)
	 */
	QByteArray popSlipFramedPacketFromStreamData(QByteArray& data) const;

	/**
	 * @brief addToLog adds a text to the log
	 * @param out true, if it was an outgoing message
	 * @param text to add to the log
	 */
	void addToLog(bool out, QString text) const;

private slots:

	/**
	 * @brief tryToConnectTCP tries to establish a connection via TCP
	 */
	void tryToConnectTCP();

	/**
     * @brief onConnected is called when the TCP socket is connected
	 */
	void onConnected();

	/**
	 * @brief onError is called when an error occurs while connected via TCP.
	 * starts a timer to retry the connection
	 */
	void onError();

	/**
	 * @brief readIncomingUdpDatagrams processes incoming UDP datagrams
	 */
	void readIncomingUdpDatagrams();

	/**
	 * @brief readIncomingTcpStream processes incoming TCP stream
	 */
	void readIncomingTcpStream();

	/**
	 * @brief processIncomingRawData processes incoming raw data and checks if it is an OSC bundle
	 * @param msgData raw OSC packet data (bundle or message)
	 */
	void processIncomingRawData(QByteArray msgData);

	/**
	 * @brief processIncomingRawMessage processes incoming single raw OSC messages
	 * @param msgData raw OSC message data (not a bundle)
	 */
	void processIncomingRawMessage(QByteArray msgData);

private:
    QStringList m_availableTypes;

    QString m_currentPresetId;

    QStringList m_presetIds;

    QMap<QString, QJsonObject> m_presets;

    QString m_currentConnectionType;

    QHostAddress m_ipAddress;

    quint16 m_udpTxPort;

    quint16 m_tcpPort;

    /**
     * @brief UDP socket
     */
    QUdpSocket				m_udpSocket;
    /**
     * @brief TCP socket
     */
    QTcpSocket				m_tcpSocket;
	/**
	 * @brief output enabled (can be overwriten by "forced" argument)
	 */
    bool					m_isEnabled;
    /**
     * @brief m_useTcp is true if TCP should be used instead of UDP
     */
    bool					m_useTcp;
	/**
	 * @brief Timer to connect again after error
	 */
    QTimer					m_tryConnectAgainTimer;
    /**
     * @brief TCP frame mode (OSC 1.0 or 1.1 SLIP)
     */
    OSCStream::EnumFrameMode m_tcpFrameMode;
	/**
	 * @brief log of incoming and / or outgoing messages
	 */
	mutable QStringList		m_log;
	/**
	 * @brief m_logIncomingMsg is true if incoming messages should be logged
	 */
	bool					m_logIncomingMsg;
	/**
	 * @brief m_logOutgoingMsg is true if outgoing messages should be logged
	 */
    bool					m_logOutgoingMsg;
	/**
	 * @brief m_incompleteStreamData may contain the begin of an incomplete OSC packet
	 * (from TCP stream)
	 */
	QByteArray				m_incompleteStreamData;

    /**
     * @brief m_logChangedSignalDelay is a timer to delay the emission of the logChanged signal
     * to prevent the log being updated to often
     */
    mutable QTimer m_logChangedSignalDelay;
};

#endif // OSCNETWORKMANAGER_H
