#include "EosCmdBlock.h"

EosCmdBlock::EosCmdBlock(MainController* controller, QString uid)
    : BlockBase(controller, uid)
    , m_userId(1)
{

}

void EosCmdBlock::getAdditionalState(QJsonObject& state) const {
    state["userId"] = getUserId();
}

void EosCmdBlock::setAdditionalState(const QJsonObject &state) {
    setUserId(state["userId"].toInt());
}
