#include "EosSubBlock.h"

#include "core/MainController.h"
#include "core/Nodes.h"

EosSubBlock::EosSubBlock(MainController *controller, QString uid)
    : OneInputBlock(controller, uid)
    , m_subNumber(1)
    , m_value(0.0)
{
    m_heightIsResizable = true;
    connect(m_inputNode, SIGNAL(dataChanged()), this, SLOT(onValueChanged()));
}

void EosSubBlock::getAdditionalState(QJsonObject& state) const {
    state["subNumber"] = getSubNumber();
    state["value"] = getValue();
}

void EosSubBlock::setAdditionalState(const QJsonObject &state) {
    setSubNumber(state["subNumber"].toInt());
    setValue(state["value"].toDouble());
}

void EosSubBlock::onValueChanged() {
    double value = m_inputNode->getValue();
    setValue(value);
}

void EosSubBlock::sendBumpEvent(double edge) {
    QString message = "/eos/sub/%1/fire";
    message = message.arg(QString::number(m_subNumber));
    m_controller->lightingConsole()->sendMessage(message, edge);
}

void EosSubBlock::setValue(double value) {
    if (value == m_value) return;
    m_value = value;
    emit valueChanged();

    QString message = "/eos/sub/%1";
    message = message.arg(QString::number(m_subNumber));
    m_controller->lightingConsole()->sendMessage(message, value);
}
