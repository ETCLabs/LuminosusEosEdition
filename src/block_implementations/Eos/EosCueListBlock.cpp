#include "EosCueListBlock.h"

#include "core/MainController.h"


EosCueListBlock::EosCueListBlock(MainController* controller, QString uid)
    : BlockBase(controller, uid)
    , m_cueListNumber(1)
{
    m_widthIsResizable = true;
    m_heightIsResizable = true;
}

void EosCueListBlock::getAdditionalState(QJsonObject& state) const {
    state["cueListNumber"] = getCueListNumber();
}

void EosCueListBlock::setAdditionalState(const QJsonObject& state) {
    setCueListNumber(state["cueListNumber"].toInt());
}
