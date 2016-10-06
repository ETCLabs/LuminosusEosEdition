#include "EosGroupBlock.h"

#include "MainController.h"
#include "Nodes.h"


EosGroupBlock::EosGroupBlock(MainController *controller, QString uid)
    : OneInputBlock(controller, uid, info().qmlFile)
    , m_groupNumber(1)
    , m_lastValue(3)
{
    connect(m_inputNode, SIGNAL(dataChanged()), this, SLOT(onValueChanged()));
}

QJsonObject EosGroupBlock::getState() const {
    QJsonObject state;
    state["groupNumber"] = getGroupNumber();
    return state;
}

void EosGroupBlock::setState(const QJsonObject &state) {
    setGroupNumber(state["groupNumber"].toInt());
}

void EosGroupBlock::onValueChanged() {
    std::vector<double> hsv = m_inputNode->getDataToRead().getHsvAt(0, 0);
    if (hsv[0] != m_lastValue[0] || hsv[1] != m_lastValue[1]) {
        QString message = "/eos/user/0/group/%1/param/hue/saturation";
        message = message.arg(QString::number(m_groupNumber));
        m_controller->eosConnection()->sendMessage(message, hsv[0] * 360, hsv[1] * 100);
    }
    if (hsv[2] != m_lastValue[2]) {
        QString message = "/eos/user/0/group/%1";
        message = message.arg(QString::number(m_groupNumber));
        m_controller->eosConnection()->sendMessage(message, hsv[2] * 100);
    }
    m_lastValue = hsv;
}
