#include "HogOSCManager.h"

#include "core/MainController.h"


HogOSCManager::HogOSCManager(MainController* controller)
    : QObject(controller)
    , m_controller(controller)
{
    connect(m_controller->lightingConsole(), SIGNAL(isConnectedChanged()),
            this, SLOT(onConnectionChanged()));
    connect(m_controller->lightingConsole(), SIGNAL(messageReceived(OSCMessage)),
            this, SLOT(onIncomingMessage(OSCMessage)));

    m_connectionTimeout.setSingleShot(true);
    m_connectionTimeout.setInterval(HogOSCManagerConstants::connectionTimeout);
    connect(&m_connectionTimeout, SIGNAL(timeout()), this, SLOT(onConnectionTimeout()));
}

void HogOSCManager::onIncomingMessage(const OSCMessage& msg) {
    if (m_controller->lightingConsole()->getCurrentType() != OscConnectionType::Hog4) return;
    if (msg.pathStartsWith("/hog/status/time")) {
        if (!m_isConnected) {
            m_isConnected = true;
            emit isConnectedChanged();
        }
        m_connectionTimeout.start();
        return;
    }
}

void HogOSCManager::onConnectionChanged() {
    // TODO: request version info?
}

void HogOSCManager::onConnectionTimeout() {
    if (m_isConnected) {
        m_isConnected = false;
        emit isConnectedChanged();
    }
    // TODO: try to reconnect?
}
