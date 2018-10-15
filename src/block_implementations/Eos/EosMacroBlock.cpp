#include "EosMacroBlock.h"

#include "core/MainController.h"
#include "core/Nodes.h"


EosMacroBlock::EosMacroBlock(MainController *controller, QString uid)
    : OneInputBlock(controller, uid)
    , m_macroNumber(1)
    , m_lastValue(0.0)
    , m_runInbackground(this, "runInBackground", false)
{
    connect(m_inputNode, SIGNAL(dataChanged()), this, SLOT(onValueChanged()));
}

void EosMacroBlock::getAdditionalState(QJsonObject& state) const {
    state["macroNumber"] = getMacroNumber();
}

void EosMacroBlock::setAdditionalState(const QJsonObject &state) {
    setMacroNumber(state["macroNumber"].toInt());
}

void EosMacroBlock::onValueChanged() {
    double value = m_inputNode->getValue();
    if (value >= LuminosusConstants::triggerThreshold && m_lastValue < LuminosusConstants::triggerThreshold) {
        fire(1.0);
    } else if (value < LuminosusConstants::triggerThreshold && m_lastValue >= LuminosusConstants::triggerThreshold) {
        fire(0.0);
    }
    m_lastValue = value;
}

void EosMacroBlock::fire(double edge) {
    QString message = "/eos/user/1/macro/%1/fire";
    if (m_runInbackground) {
        message = "/eos/user/0/macro/%1/fire";
    }
    message = message.arg(QString::number(m_macroNumber));
    m_controller->lightingConsole()->sendMessage(message, edge);
}
