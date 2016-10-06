#include "EosCmdBlock.h"

EosCmdBlock::EosCmdBlock(MainController* controller, QString uid)
    : BlockBase(controller, uid, info().qmlFile)
    , m_userId(1)
{

}

QJsonObject EosCmdBlock::getState() const {
    QJsonObject state;
    state["userId"] = getUserId();
    return state;
}

void EosCmdBlock::setState(const QJsonObject &state) {
    setUserId(state["userId"].toInt());
}
