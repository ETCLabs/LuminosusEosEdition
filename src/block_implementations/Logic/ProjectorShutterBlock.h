#ifndef PROJECTORSHUTTERBLOCK_H
#define PROJECTORSHUTTERBLOCK_H

#include "core/block_data/OneInputBlock.h"
#include "core/SmartAttribute.h"

#include <QTcpSocket>
#include <QTimer>


class ProjectorShutterBlock : public OneInputBlock {
    Q_OBJECT

public:

    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "PJLink Projector";
        info.nameInUi = "Projector (PJLink)";
        info.category << "General" << "Media + Presentation";
        info.helpText = "Controls a projector via the network protocol PJLink.\n\n"
                        "The 'Turn On' and 'Turn Off' buttons need to be long clicked to take effect.\n"
                        "'Shutter' means in this case 'AV Mute'.\n\n"
                        "The default port is 4352, the default password can be found in the manual "
                        "of the projector and the IP address can usually be found in the projector "
                        "settings -> 'Network'.";
        info.qmlFile = "qrc:/qml/Blocks/Logic/ProjectorShutterBlock.qml";
        info.complete<ProjectorShutterBlock>();
        return info;
    }

    explicit ProjectorShutterBlock(MainController* controller, QString uid);

signals:

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    void checkConnection();
    void resetConnection();

    void checkPower();
    void checkAvMute();

    void turnAvMuteOff();

    void turnAvMuteOn();

    void toggleAvMute();

    void turnOn();

    void turnOff();

private slots:
    void sendCommand(QString command);

    void connectToProjector();

    void onConnectionStateChanged(QAbstractSocket::SocketState socketState);

    void onMessageReceived();

protected:
    QPointer<NodeBase> m_onNode;
    QPointer<NodeBase> m_offNode;
    QPointer<NodeBase> m_openShutterNode;

    QTcpSocket* m_tcpSocket;
    QTimer m_connectionCheckTimer;
    QTimer m_powerStatusTimer;
    QString m_authenticationToken;

    StringAttribute m_ipAddress;
    IntegerAttribute m_port;
    StringAttribute m_password;

    StringAttribute m_projectorName;
    BoolAttribute m_shutterIsOpen;
    BoolAttribute m_connected;
    BoolAttribute m_authenticated;
    BoolAttribute m_passwordIsWrong;
    IntegerAttribute m_powerStatus;

};

#endif // PROJECTORSHUTTERBLOCK_H
