#include "EosEffectBlock.h"

#include "core/MainController.h"
#include "core/Nodes.h"


EosEffectBlock::EosEffectBlock(MainController *controller, QString uid)
    : OneInputBlock(controller, uid)
    , m_effectNumber(1)
    , m_lastValue(120.0)
    , m_sendRegularly(this, "sendRegularly", true)
{
    connect(m_inputNode, SIGNAL(dataChanged()), this, SLOT(onValueChanged()));
    connect(m_controller->eosManager(), &EosOSCManager::connectionEstablished, this, &EosEffectBlock::sendBpm);

    m_timer.setInterval(1000);
    connect(&m_timer, &QTimer::timeout, [this](){ if (m_sendRegularly) this->sendBpm(); });
    m_timer.start();
}

void EosEffectBlock::getAdditionalState(QJsonObject& state) const {
    state["effectNumber"] = getEffectNumber();
}

void EosEffectBlock::setAdditionalState(const QJsonObject &state) {
    setEffectNumber(state["effectNumber"].toDouble());
}

void EosEffectBlock::onValueChanged() {
    double value = m_inputNode->getAbsoluteValue();
    if (value <= 0) return;
    double bpm = 60.0 / value;
    if (int(bpm*10) == int(m_lastValue*10)) return;
    m_lastValue = bpm;
    sendBpm();
}

void EosEffectBlock::sendBpm() {
    QString message = "/eos/user/0/newcmd/Effect/%1/BPM/%2/#";
    message = message.arg(QString::number(m_effectNumber, 'f', 2), QString::number(m_lastValue, 'f', 1));
    m_controller->lightingConsole()->sendMessage(message);
}
