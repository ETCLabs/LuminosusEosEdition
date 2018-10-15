#include "ProjectorShutterBlock.h"

#include "core/Nodes.h"

#include <QHostAddress>
#include <QCryptographicHash>


QString md5(QString input) {
    QString result = QString(QCryptographicHash::hash(input.toUtf8(), QCryptographicHash::Md5).toHex());
    return result;
}


ProjectorShutterBlock::ProjectorShutterBlock(MainController* controller, QString uid)
    : OneInputBlock(controller, uid)
    , m_onNode(nullptr)
    , m_offNode(nullptr)
    , m_openShutterNode(nullptr)
    , m_tcpSocket(new QTcpSocket())
    , m_ipAddress(this, "ipAddress", "")
    , m_port(this, "port", 4352, 1, 65535)
    , m_password(this, "password", "")
    , m_projectorName(this, "projectorName", "", /*persistent=*/ false)
    , m_shutterIsOpen(this, "shutterIsOpen", true, /*persistent=*/ false)
    , m_connected(this, "connected", false, /*persistent=*/ false)
    , m_authenticated(this, "authenticated", false, /*persistent=*/ false)
    , m_passwordIsWrong(this, "passwordIsWrong", false, /*persistent=*/ false)
    , m_powerStatus(this, "powerStatus", 0, /*persistent=*/ false)
{
    m_onNode = createInputNode("on");
    m_onNode->enableImpulseDetection();
    connect(m_onNode, SIGNAL(impulseBegin()), this, SLOT(turnOn()));

    m_offNode = createInputNode("off");
    m_offNode->enableImpulseDetection();
    connect(m_offNode, SIGNAL(impulseBegin()), this, SLOT(turnOff()));

    m_openShutterNode = createInputNode("openShutter");
    m_openShutterNode->enableImpulseDetection();
    connect(m_openShutterNode, SIGNAL(impulseBegin()), this, SLOT(turnAvMuteOff()));

    m_inputNode->enableImpulseDetection();
    connect(m_inputNode, SIGNAL(impulseBegin()), this, SLOT(turnAvMuteOn()));

    m_connectionCheckTimer.setInterval(3*1000);
    m_connectionCheckTimer.setSingleShot(false);
    connect(&m_connectionCheckTimer, &QTimer::timeout, this, &ProjectorShutterBlock::checkConnection);
    m_connectionCheckTimer.start();

    // it is required to send a command at least every 30s to hold the connection open
    // therefore we will query the power status every 5s:
    m_powerStatusTimer.setInterval(5*1000);
    m_powerStatusTimer.setSingleShot(false);
    connect(&m_powerStatusTimer, &QTimer::timeout, this, &ProjectorShutterBlock::checkPower);
    m_powerStatusTimer.start();

    connect(m_tcpSocket, &QTcpSocket::stateChanged, this, &ProjectorShutterBlock::onConnectionStateChanged);
    connect(m_tcpSocket, &QTcpSocket::readyRead, this, &ProjectorShutterBlock::onMessageReceived);
}

void ProjectorShutterBlock::checkConnection() {
    if (m_tcpSocket->state() == QAbstractSocket::UnconnectedState
            || (m_tcpSocket->state() == QAbstractSocket::ConnectedState
                && (m_tcpSocket->peerAddress() != QHostAddress(m_ipAddress)
                    || m_tcpSocket->peerPort() != m_port))) {
        if (m_tcpSocket->state() == QAbstractSocket::ConnectedState) {
            qDebug() << "Disconnecting from projector due to address change...";
            m_tcpSocket->disconnectFromHost();
        }
        m_tcpSocket->connectToHost(m_ipAddress, quint16(m_port));
        m_projectorName = "";
        m_powerStatus = 0;
    }
}

void ProjectorShutterBlock::resetConnection() {
    m_tcpSocket->abort();
    m_tcpSocket->connectToHost(m_ipAddress, quint16(m_port));
    m_projectorName = "";
    m_powerStatus = 0;
    m_passwordIsWrong = false;
}

void ProjectorShutterBlock::checkPower() {
    sendCommand("%1POWR ?\r");
}

void ProjectorShutterBlock::checkAvMute() {
    sendCommand("%1AVMT ?\r");
}

void ProjectorShutterBlock::turnAvMuteOff() {
    sendCommand("%1AVMT 30\r");
    m_shutterIsOpen = true;
}

void ProjectorShutterBlock::turnAvMuteOn() {
    sendCommand("%1AVMT 31\r");
    m_shutterIsOpen = false;
}

void ProjectorShutterBlock::toggleAvMute() {
    if (m_shutterIsOpen) {
        turnAvMuteOn();
    } else {
        turnAvMuteOff();
    }
}

void ProjectorShutterBlock::turnOn() {
    sendCommand("%1POWR 1\r");
    m_powerStatus = 1;
}

void ProjectorShutterBlock::turnOff() {
    sendCommand("%1POWR 0\r");
    m_powerStatus = 0;
}

void ProjectorShutterBlock::sendCommand(QString command) {
    if (!m_connected || !m_authenticated) return;
    m_tcpSocket->write((m_authenticationToken + command).toUtf8());
}

void ProjectorShutterBlock::connectToProjector() {
    if (m_tcpSocket->isOpen()) {
        m_tcpSocket->disconnectFromHost();
        m_connected = false;
    }
    m_tcpSocket->connectToHost(m_ipAddress, quint16(m_port));
}

void ProjectorShutterBlock::onConnectionStateChanged(QAbstractSocket::SocketState socketState) {
    if (socketState == QAbstractSocket::ConnectedState) {
        m_connected = true;
        m_authenticated = false;
    } else if (socketState == QAbstractSocket::UnconnectedState) {
        m_connected = false;
        m_authenticated = false;
    }
}

void ProjectorShutterBlock::onMessageReceived() {
    QByteArray data = m_tcpSocket->readAll();

    if (data.startsWith(QString("PJLINK 0").toUtf8())) {
        // -> connection established, no authentication required
        m_authenticationToken = "";
        m_authenticated = true;
        m_passwordIsWrong = false;
        sendCommand("%1NAME ?\r");

    } else if (data.startsWith(QString("PJLINK 1").toUtf8())) {
        // -> connection established, received random number for authentification
        if (data.size() < 18) {
            qWarning() << "Received invalid response from projector.";
            return;
        }
        QString randomNumber = data.mid(9, 8);
        m_authenticationToken = md5(randomNumber + m_password);
        m_authenticated = true;
        m_passwordIsWrong = false;
        sendCommand("%1NAME ?\r");

    } else if (data.startsWith(QString("PJLINK ERRA").toUtf8())) {
        m_authenticated = false;
        m_passwordIsWrong = true;

    } else if (data.startsWith(QString("%1NAME=").toUtf8())) {
        m_projectorName = QString::fromUtf8(data.mid(7, data.size() - 8));
        checkPower();

    } else if (data.endsWith(QString("=OK\r").toUtf8())) {
        // ignore
    } else if (data.endsWith(QString("=ERR1\r").toUtf8())) {
        qDebug() << "Projector reports undefined command.";
    } else if (data.endsWith(QString("=ERR2\r").toUtf8())) {
        qDebug() << "Projector reports out-of-parameter.";
    } else if (data.endsWith(QString("=ERR3\r").toUtf8())) {
        qWarning() << "Projector unavailable.";
    } else if (data.endsWith(QString("=ERR4\r").toUtf8())) {
        qWarning() << "Projector reports failure.";
    } else if (data.startsWith(QString("%1POWR=").toUtf8())) {
        if (data.size() < 9) {
            qWarning() << "Projector responded with invalid power status.";
            return;
        }
        m_powerStatus = QString(data.mid(7, 1)).toInt();

    } else if (data.startsWith(QString("%1AVMT=").toUtf8())) {
        if (data.size() < 10) {
            qWarning() << "Projector responded with invalid shutter status.";
            return;
        }
        m_shutterIsOpen = data.mid(7, 2) == QString("30").toUtf8() // audio and video mute OFF
                || data.mid(7, 2) == QString("21").toUtf8();  // audio mute ON video mute OFF

    } else {
        qDebug() << "Received response from projector:" << QString::fromUtf8(data);
    }
}
