#include "EosEffectBlock.h"

#include "MainController.h"
#include "Nodes.h"


EosEffectBlock::EosEffectBlock(MainController *controller, QString uid)
    : OneInputBlock(controller, uid, info().qmlFile)
    , m_effectNumber(1)
    , m_lastValue(0.0)
{
    connect(m_inputNode, SIGNAL(dataChanged()), this, SLOT(onValueChanged()));
}

QJsonObject EosEffectBlock::getState() const {
    QJsonObject state;
    state["effectNumber"] = getEffectNumber();
    return state;
}

void EosEffectBlock::setState(const QJsonObject &state) {
    setEffectNumber(state["effectNumber"].toDouble());
}

void EosEffectBlock::onValueChanged() {
    double value = m_inputNode->getAbsoluteValue();
    if (int(value*10) == int(m_lastValue*10) || value <= 0) return;
    m_lastValue = value;
    double bpm = 60.0 / value;

    QString message = "/eos/user/0/newcmd/Effect/%1/BPM/%2/#";
    message = message.arg(QString::number(m_effectNumber, 'f', 2), QString::number(bpm, 'f', 1));
    m_controller->eosConnection()->sendMessage(message);
}
