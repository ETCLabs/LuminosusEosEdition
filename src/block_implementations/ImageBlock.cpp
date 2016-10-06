#include "ImageBlock.h"

#include "MainController.h"


ImageBlock::ImageBlock(MainController* controller, QString uid)
    : BlockBase(controller, uid, info().qmlFile)
    , m_filePath("")
    , m_rotation(0)
    , m_shadow(false)
{

}

QJsonObject ImageBlock::getState() const {
    QJsonObject state;
    double dp = m_controller->getGuiScaling();
    state["filePath"] = getFilePath();
    state["rotation"] = getRotation();
    state["shadow"] = getShadow();
    state["guiItemWidth"] = getGuiItemConst()->width() / dp;
    return state;
}

void ImageBlock::setState(const QJsonObject &state) {
    double dp = m_controller->getGuiScaling();
    setFilePath(state["filePath"].toString());
    setRotation(state["rotation"].toDouble());
    setShadow(state["shadow"].toBool());
    if (state["guiItemWidth"].toDouble() > 0) {
        getGuiItem()->setWidth(state["guiItemWidth"].toDouble() * dp);
    }
}
