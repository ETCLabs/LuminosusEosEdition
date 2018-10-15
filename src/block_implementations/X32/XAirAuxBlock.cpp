#include "XAirAuxBlock.h"

#include "core/MainController.h"


XAirAuxBlock::XAirAuxBlock(MainController* controller, QString uid)
    : OneInputBlock(controller, uid)
    , m_panNode(nullptr)
    , m_onNode(nullptr)
    , m_channelNumber(this, "channelNumber", 1, 1, 32)
    , m_bus(this, "bus", 0, 0, 16)
    , m_faderPos(this, "faderPos", 0.0, 0, 1)
    , m_pan(this, "pan", 0.5)
    , m_boost(this, "boost", true)
    , m_on(this, "on", true)
    , m_pauseValueTransmission(false)
{
    m_heightIsResizable = true;

    m_panNode = createInputNode("panNode");
    m_onNode = createInputNode("onNode");

    m_onNode->enableImpulseDetection();
    connect(m_onNode, &NodeBase::impulseBegin, [this](){ m_on = true; });
    connect(m_onNode, &NodeBase::impulseEnd, [this](){ m_on = false; });

    m_subscribeRefreshTimer.setInterval(10000);  // 10s
    connect(&m_subscribeRefreshTimer, SIGNAL(timeout()), this, SLOT(updateSubscription()));
    m_subscribeRefreshTimer.start();

    connect(&m_faderPos, SIGNAL(valueChanged()), this, SIGNAL(decibelChanged()));

    connect(&m_label, SIGNAL(valueChanged()), this, SLOT(sendName()));
    connect(&m_faderPos, SIGNAL(valueChanged()), this, SLOT(sendFaderPos()));
    connect(&m_pan, SIGNAL(valueChanged()), this, SLOT(sendPan()));
    connect(&m_on, SIGNAL(valueChanged()), this, SLOT(sendOn()));

    connect(&m_channelNumber, SIGNAL(valueChanged()), this, SLOT(retrieveStateFromConsole()));
    connect(&m_bus, SIGNAL(valueChanged()), this, SLOT(retrieveStateFromConsole()));

    connect(m_inputNode, &NodeBase::dataChanged, [this](){ m_faderPos.setValue(m_inputNode->getValue()); });
    connect(m_panNode, &NodeBase::dataChanged, [this](){ m_pan.setValue(m_panNode->getValue()); });

    connect(controller->audioConsole(), SIGNAL(messageReceived(OSCMessage)), this, SLOT(onMessageReceived(OSCMessage)));

}

void XAirAuxBlock::setState(const QJsonObject& state) {
    m_pauseValueTransmission = true;
    readAttributesFrom(state);
    setAdditionalState(state);
    m_pauseValueTransmission = false;
}

void XAirAuxBlock::sendFaderPos() {
    if (m_pauseValueTransmission) return;

    QString message = "/rtn/aux/mix";
    if (m_bus == 0) {
        message.append("/fader");
    } else {
        message.append("/%1/level");
        message = message.arg(m_bus, 2, 10, QChar('0'));
    }

    double value = m_boost ? m_faderPos : m_faderPos * 0.75;

    m_controller->audioConsole()->sendMessage32bit(message, value);
}

void XAirAuxBlock::sendName() {
    if (m_pauseValueTransmission) return;
    QString message = "/rtn/aux/config/name";
    // max. name length is 12
    m_controller->audioConsole()->sendMessage(message, m_label.getValue().left(12));
}

void XAirAuxBlock::sendPan() {
    if (m_pauseValueTransmission) return;
    QString message = "/rtn/aux/mix/pan";
    m_controller->audioConsole()->sendMessage32bit(message, m_pan);
}

void XAirAuxBlock::sendOn() {
    if (m_pauseValueTransmission) return;
    QString message = "/rtn/aux/mix/on";
    m_controller->audioConsole()->sendMessage32bit(message, m_on ? 1 : 0);
}

double XAirAuxBlock::getDecibel() const {
    double f = m_boost ? m_faderPos : m_faderPos * 0.75;
    double d = 0.0;

    if (f >= 0.5)
        d = f * 40. - 30.;
    else if (f >= 0.25)
        d = f * 80. - 50.;
    else if (f >= 0.0625)
        d = f * 160. - 70.;
    else if (f >= 0.0)
        d = f * 480. - 90.;

    return d;
}

void XAirAuxBlock::retrieveStateFromConsole() {
    // get fader position:
    QString faderPosMessage = "/rtn/aux/mix";
    if (m_bus == 0) {
        faderPosMessage.append("/fader");
    } else {
        faderPosMessage.append("/%1/level");
        faderPosMessage = faderPosMessage.arg(m_bus, 2, 10, QChar('0'));
    }
    m_controller->audioConsole()->sendMessage(faderPosMessage);

    // get label:
    QString message = "/rtn/aux/config/name";
    m_controller->audioConsole()->sendMessage(message);

    // get pan:
    message = "/rtn/aux/mix/pan";
    m_controller->audioConsole()->sendMessage(message);

    // get on state:
    message = "/rtn/aux/mix/on";
    m_controller->audioConsole()->sendMessage(message);

    updateSubscription();
}

void XAirAuxBlock::updateSubscription() {
    // subscribe for fader position:
    QString faderPosMessage = "/rtn/aux/mix";
    if (m_bus == 0) {
        faderPosMessage.append("/fader");
    } else {
        faderPosMessage.append("/%1/level");
        faderPosMessage = faderPosMessage.arg(m_bus, 2, 10, QChar('0'));
    }
    // ",4" at the end means every 4th message of 50 messages / second
    m_controller->audioConsole()->sendMessage("/subscribe=" + faderPosMessage + ",4");

    // subscribe for label:
    QString message = "/rtn/aux/config/name";
    m_controller->audioConsole()->sendMessage("/subscribe=" + message + ",50");

    // subscribe for pan:
    message = "/rtn/aux/mix/pan";
    m_controller->audioConsole()->sendMessage("/subscribe=" + message + ",20");

    // subscribe for on state:
    message = "/rtn/aux/mix/on";
    m_controller->audioConsole()->sendMessage("/subscribe=" + message + ",10");
}

void XAirAuxBlock::onMessageReceived(OSCMessage msg) {
    QString channelMessageBegin = "/rtn/aux/";

    if (!msg.pathStartsWith(channelMessageBegin)) return;

    // fader pos:
    QString faderPosMessage = "/rtn/aux/mix";
    if (m_bus == 0) {
        faderPosMessage.append("/fader");
    } else {
        faderPosMessage.append("/%1/level");
        faderPosMessage = faderPosMessage.arg(m_bus, 2, 10, QChar('0'));
    }

    if (msg.pathString() == faderPosMessage) {
        m_pauseValueTransmission = true;
        double value = msg.value();
        if (value > 0.75) {
            m_boost = true;
        }
        m_faderPos = m_boost ? value : value / 0.75;
        m_pauseValueTransmission = false;
        return;
    }

    // check if msg is "/ch/%1/config/name" -> label:
    if (msg.pathPart(2) == "config" && msg.pathPart(3) == "name") {
        m_pauseValueTransmission = true;
        m_label = msg.stringValue();
        m_pauseValueTransmission = false;
        return;
    }

    // check if msg is "/ch/%1/config/name" -> label:
    if (msg.pathPart(2) == "mix" && msg.pathPart(3) == "pan") {
        m_pauseValueTransmission = true;
        m_pan = msg.value();
        m_pauseValueTransmission = false;
        return;
    }

    // on state:
    QString onMessage = "/rtn/aux/mix/on";
    if (msg.pathString() == onMessage) {
        m_pauseValueTransmission = true;
        m_on = msg.isTrue();
        m_pauseValueTransmission = false;
        return;
    }
}
