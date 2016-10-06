#include "EosMacroBlock.h"

#include "MainController.h"
#include "Nodes.h"


EosMacroBlock::EosMacroBlock(MainController *controller, QString uid)
    : OneInputBlock(controller, uid, info().qmlFile)
    , m_macroNumber(1)
    , m_lastValue(0.0)
{
    connect(m_inputNode, SIGNAL(dataChanged()), this, SLOT(onValueChanged()));
}

QJsonObject EosMacroBlock::getState() const {
    QJsonObject state;
    state["macroNumber"] = getMacroNumber();
    return state;
}

void EosMacroBlock::setState(const QJsonObject &state) {
    setMacroNumber(state["macroNumber"].toInt());
}

void EosMacroBlock::onValueChanged() {
    double value = m_inputNode->getValue();
    QString message = "/eos/macro/%1/fire";
    message = message.arg(QString::number(m_macroNumber));
    if (value >= LuminosusConstants::triggerThreshold && m_lastValue < LuminosusConstants::triggerThreshold) {
        m_controller->eosConnection()->sendMessage(message, 1.0);
    } else if (value < LuminosusConstants::triggerThreshold && m_lastValue >= LuminosusConstants::triggerThreshold) {
        m_controller->eosConnection()->sendMessage(message, 0.0);
    }
    m_lastValue = value;
}

void EosMacroBlock::fire(double edge) {
    QString message = "/eos/macro/%1/fire";
    message = message.arg(QString::number(m_macroNumber));
    m_controller->eosConnection()->sendMessage(message, edge);
}
