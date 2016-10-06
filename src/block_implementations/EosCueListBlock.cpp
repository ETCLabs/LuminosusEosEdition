#include "EosCueListBlock.h"

#include "MainController.h"


EosCueListBlock::EosCueListBlock(MainController* controller, QString uid)
    : BlockBase(controller, uid, info().qmlFile)
    , m_cueListNumber(1)
{

}

QJsonObject EosCueListBlock::getState() const {
    QJsonObject state;
    double dp = m_controller->getGuiScaling();
    state["cueListNumber"] = getCueListNumber();
    state["guiItemWidth"] = getGuiItemConst()->width() / dp;
    state["guiItemHeight"] = getGuiItemConst()->height() / dp;
    return state;
}

void EosCueListBlock::setState(const QJsonObject& state) {
    double dp = m_controller->getGuiScaling();
    setCueListNumber(state["cueListNumber"].toInt());
    if (state["guiItemWidth"].toDouble() > 0) {
        getGuiItem()->setWidth(state["guiItemWidth"].toDouble() * dp);
    }
    if (state["guiItemHeight"].toDouble() > 0) {
        getGuiItem()->setHeight(state["guiItemHeight"].toDouble() * dp);
    }
}
