#include "X32Manager.h"

#include "core/MainController.h"


X32Manager::X32Manager(MainController* controller)
    : QObject(controller)
    , m_controller(controller)
    , m_isConnected(false)
    , m_infoTimeout(this)
    , m_requestConsoleInfoTimer(this)
{
    m_infoTimeout.setSingleShot(true);
    m_infoTimeout.setInterval(X32ManagerConstants::infoTimeout);
    connect(&m_infoTimeout, SIGNAL(timeout()), this, SLOT(onInfoTimeout()));

    m_requestConsoleInfoTimer.setSingleShot(false);
    m_requestConsoleInfoTimer.setInterval(X32ManagerConstants::requestConsoleInfoInterval);
    connect(&m_requestConsoleInfoTimer, SIGNAL(timeout()), this, SLOT(requestConsoleInfo()));
    m_requestConsoleInfoTimer.start();

    connect(m_controller->audioConsole(), SIGNAL(currentPresetChanged()),
            this, SLOT(requestConsoleInfo()));
    connect(m_controller->audioConsole(), SIGNAL(isConnectedChanged()),
            this, SLOT(requestConsoleInfo()));
    connect(m_controller->audioConsole(), SIGNAL(messageReceived(OSCMessage)),
            this, SLOT(onIncomingMessage(OSCMessage)));
}

X32Manager::~X32Manager() {
    m_infoTimeout.stop();
    m_requestConsoleInfoTimer.stop();
}

void X32Manager::onIncomingMessage(const OSCMessage& msg) {

    if (msg.pathPart(0) == "info" && msg.arguments().size() >= 4) {
        m_consoleInfoName = msg.arguments().at(1).toString();
        m_consoleInfoModel = msg.arguments().at(2).toString();
        m_consoleInfoVersion = msg.arguments().at(3).toString();
        emit consoleInfoChanged();

        m_isConnected = true;
        emit isConnectedChanged();
        m_infoTimeout.start();
    }
}

void X32Manager::requestConsoleInfo() {
    m_controller->audioConsole()->sendMessage("/info");
}

void X32Manager::onInfoTimeout() {
    m_isConnected = false;
    emit isConnectedChanged();

    m_consoleInfoName = "";
    m_consoleInfoModel = "";
    m_consoleInfoVersion = "";
    emit consoleInfoChanged();
}
