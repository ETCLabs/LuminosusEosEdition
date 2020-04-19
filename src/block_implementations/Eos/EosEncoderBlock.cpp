#include "EosEncoderBlock.h"

#include "core/MainController.h"
#include "core/Nodes.h"


EosEncoderBlock::EosEncoderBlock(MainController* controller, QString uid)
    : BlockBase(controller, uid)
    , m_target(1)
    , m_channel(1)
    , m_useDefaultChannel(true)
    , m_learning(false)
    , m_parameterName(this, "parameterName", "pan")
    , m_fineMode(this, "fineMode", false, /*persistent*/ false)
    , m_active(this, "active", false, true)
    , m_accelerate(this, "accelerate", true, true)
    , m_feedbackEnabled(this, "feedback", true, true)
{
    connect(m_controller->midi(), SIGNAL(messageReceived(MidiEvent)), this, SLOT(onMidiMessage(MidiEvent)));
    connect(&m_feedbackEnabled, SIGNAL(valueChanged()), this, SLOT(onFeedbackEnabledChanged()));

    emit(m_feedbackEnabled.valueChanged());
}

void EosEncoderBlock::getAdditionalState(QJsonObject& state) const {
    state["target"] = getTarget();
    state["channel"] = getChannel();
    state["useDefaultChannel"] = getUseDefaultChannel();
}

void EosEncoderBlock::setAdditionalState(const QJsonObject &state) {
    setTarget(state["target"].toInt());
    setChannel(state["channel"].toInt());
    setUseDefaultChannel(state["useDefaultChannel"].toBool());
}

void EosEncoderBlock::onMidiMessage(MidiEvent event) {
    // from improbable to probable
    // check if this is a ControlChange message and target is correct:
    if (event.type == MidiConstants::CONTROL_CHANGE
            && event.target == m_target) {
        // check if channel is correct:
        int channel = m_useDefaultChannel ? m_controller->midi()->getDefaultInputChannel() : m_channel;
        if (channel == MidiConstants::OMNI_MODE_CHANNEL || event.channel == channel) {
            // channel and target are correct
            // set value:
            double relativeValue = event.value * 127.0;
            if (relativeValue > 64) {
                // value is 7-bit twos complement
                relativeValue -= 128;
            }
            // accelerate:
            if (m_accelerate)
                relativeValue = (qAbs(relativeValue) > 1) ? relativeValue * 2 : relativeValue;

            QString message;
            if (!m_active)
                message = m_fineMode ? "/eos/user/1/wheel/fine/%1" : "/eos/user/1/wheel/coarse/%1";
            else
                message = m_fineMode ? "/eos/active/wheel/fine/%1" : "/eos/active/wheel/coarse/%1";

            message = message.arg(m_parameterName);
            m_controller->lightingConsole()->sendMessage(message, relativeValue);
            emit validMessageReceived();
        }
    }
}

void EosEncoderBlock::startLearning() {
    // check if already in learning state:
    if (m_learning) {
        // yes -> cancel learning:
        m_controller->midi()->removeNextEventCallback(getUid());
        setLearning(false);
        return;
    }
    // listen for the next event:
    setLearning(true);
    m_controller->midi()->registerForNextEvent(getUid(), [this](MidiEvent event) { this->checkIfEventFits(event); });
}

void EosEncoderBlock::checkIfEventFits(MidiEvent event) {
    setLearning(false);
    // check if this event was a control change event:
    if (event.type != MidiConstants::CONTROL_CHANGE) {
        m_controller->guiManager()->showToast("This was not a Control Change event.");
        return;
    }
    // set attributes to match the event:
    setUseDefaultChannel(false);
    setChannel(event.channel);
    setTarget(event.target);
    emit validMessageReceived();
}

void EosEncoderBlock::onIncomingEosMessage(const EosOSCMessage& msg) {
    if (msg.pathPart(0) != "active" && msg.pathPart(1) != "wheel") return;

    int wheelIndex = msg.pathPart(2).toInt();
    if (wheelIndex != m_parameterName.getValue().toInt()) return;
    QStringList arg = msg.stringValue().split("  [");
    QString label;
    int value = 0;
    if (arg.size() == 2) {
        label = arg[0];
        value = arg[1].remove("]").toInt();

        m_controller->midi()->sendChannelVoiceMessage(MidiConstants::CONTROL_CHANGE,
                                                      m_channel, m_target, (double) value / 100);
    }
}

void EosEncoderBlock::onFeedbackEnabledChanged() {
    if (m_feedbackEnabled)
        connect(m_controller->eosManager(), SIGNAL(eosMessageReceived(EosOSCMessage)),
                this, SLOT(onIncomingEosMessage(EosOSCMessage)));
    else
        disconnect(m_controller->eosManager(), SIGNAL(eosMessageReceived(EosOSCMessage)),
                this, SLOT(onIncomingEosMessage(EosOSCMessage)));
}
