#include "EosSubBlock.h"

#include "MainController.h"
#include "Nodes.h"

EosSubBlock::EosSubBlock(MainController *controller, QString uid)
    : OneInputBlock(controller, uid, info().qmlFile)
    , m_subNumber(1)
    , m_value(0.0)
{
    connect(m_inputNode, SIGNAL(dataChanged()), this, SLOT(onValueChanged()));
}

QJsonObject EosSubBlock::getState() const {
    QJsonObject state;
    double dp = m_controller->getGuiScaling();
    state["subNumber"] = getSubNumber();
    state["value"] = getValue();
    state["guiItemHeight"] = getGuiItemConst()->height() / dp;
    return state;
}

void EosSubBlock::setState(const QJsonObject &state) {
    setSubNumber(state["subNumber"].toInt());
    setValue(state["value"].toDouble());
    if (state["guiItemHeight"].toDouble() > 0) {
        double dp = m_controller->getGuiScaling();
        getGuiItem()->setHeight(state["guiItemHeight"].toDouble() * dp);
    }
}

void EosSubBlock::onValueChanged() {
    double value = m_inputNode->getValue();
    setValue(value);
}

void EosSubBlock::sendBumpEvent(double edge) {
    QString message = "/eos/sub/%1/fire";
    message = message.arg(QString::number(m_subNumber));
    m_controller->eosConnection()->sendMessage(message, edge);
}

void EosSubBlock::setValue(double value) {
    if (value == m_value) return;
    m_value = value;
    emit valueChanged();

    QString message = "/eos/sub/%1";
    message = message.arg(QString::number(m_subNumber));
    m_controller->eosConnection()->sendMessage(message, value);
}
