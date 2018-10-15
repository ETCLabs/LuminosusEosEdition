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
{
    connect(m_controller->midi(), SIGNAL(messageReceived(MidiEvent)), this, SLOT(onMidiMessage(MidiEvent)));
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
            relativeValue = (qAbs(relativeValue) > 1) ? relativeValue * 2 : relativeValue;
            QString message = m_fineMode ? "/eos/user/1/wheel/fine/%1" : "/eos/user/1/wheel/coarse/%1";
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

