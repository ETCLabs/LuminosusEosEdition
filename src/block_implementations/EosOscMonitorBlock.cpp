#include "EosOscMonitorBlock.h"

#include "MainController.h"


EosOscMonitorBlock::EosOscMonitorBlock(MainController* controller, QString uid)
    : BlockBase(controller, uid, info().qmlFile)
{

}

QJsonObject EosOscMonitorBlock::getState() const {
    QJsonObject state;
    double dp = m_controller->getGuiScaling();
    state["guiItemHeight"] = getGuiItemConst()->height() / dp;
    state["guiItemWidth"] = getGuiItemConst()->width() / dp;
    return state;
}

void EosOscMonitorBlock::setState(const QJsonObject& state) {
    double dp = m_controller->getGuiScaling();
    if (state["guiItemHeight"].toDouble() > 0) {
        getGuiItem()->setHeight(state["guiItemHeight"].toDouble() * dp);
    }
    if (state["guiItemWidth"].toDouble() > 0) {
        getGuiItem()->setWidth(state["guiItemWidth"].toDouble() * dp);
    }
}
