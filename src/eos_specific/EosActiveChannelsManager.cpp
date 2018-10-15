#include "EosActiveChannelsManager.h"

#include "core/MainController.h"


EosActiveChannelsManager::EosActiveChannelsManager(MainController* controller)
    : QObject(controller)
    , m_controller(controller)
    , m_wheelLabels(0)
    , m_wheelValues(0)
{
    connect(controller->eosManager(), SIGNAL(connectionEstablished()),
            this, SLOT(onConnectionEstablished()));
    connect(controller->eosManager(), SIGNAL(eosMessageReceived(EosOSCMessage)),
            this, SLOT(onIncomingEosMessage(EosOSCMessage)));
}

void EosActiveChannelsManager::onConnectionEstablished() {

}

void EosActiveChannelsManager::onIncomingEosMessage(const EosOSCMessage& msg) {
    // all messages related to the currently selected / active channels
    // start with /eos/out/active -> ignore all other:
    if (msg.pathPart(0) != "active") return;

    if (msg.pathPart(1) == "chan") {
        // this message contains information about the currently selected / active channel
        // /eos/out/active/chan = "<number/range> [<intensity 1st ch>] <fixture 1st ch> @ <int address 1st ch>
        setActiveChannelsLabel(msg.stringValue());
    } else if (msg.pathPart(1) == "wheel") {
        // this message contains a string with the label and the value of a wheel
        // /eos/out/active/wheel/<index + 1> = "<label> [<value>]"

        // separate number, label and value:
        int wheelIndex = msg.pathPart(2).toInt() - 1;
        if (wheelIndex < 0 || wheelIndex > 100) return;
        QStringList arg = msg.stringValue().split("  [");
        QString label;
        int value = 0;
        if (arg.size() == 2) {
            label = arg[0];
            value = arg[1].remove("]").toInt();
        }

        // update values:
        if (label.isEmpty()) {
            // remove this wheel and all follwing:
            if (m_wheelLabels.size() > wheelIndex) {
                m_wheelLabels.resize(wheelIndex);
                m_wheelValues.resize(wheelIndex);
            }
        } else {
            if (m_wheelLabels.size() <= wheelIndex) {
                m_wheelLabels.resize(wheelIndex + 1);
                m_wheelValues.resize(wheelIndex + 1);
            }
            m_wheelLabels[wheelIndex] = label;
            m_wheelValues[wheelIndex] = value;
        }
        emit wheelsChanged();
    }
}

void EosActiveChannelsManager::startChangeParameter(int index, double tickValue) {
    QString message = "/eos/active/switch/%1";
    message = message.arg(QString::number(index + 1));
    m_controller->lightingConsole()->sendMessage(message, tickValue);
}

void EosActiveChannelsManager::stopChangeParameter(int index) {
    QString message = "/eos/active/switch/%1";
    message = message.arg(QString::number(index + 1));
    m_controller->lightingConsole()->sendMessage(message, 0.0);
}
