#include "ImageBlock.h"

#include "core/MainController.h"


ImageBlock::ImageBlock(MainController* controller, QString uid)
    : BlockBase(controller, uid)
    , m_filePath("")
    , m_rotation(0)
    , m_shadow(false)
{
    m_widthIsResizable = true;
}

void ImageBlock::getAdditionalState(QJsonObject& state) const {
    state["filePath"] = getFilePath();
    state["rotation"] = getRotation();
    state["shadow"] = getShadow();
}

void ImageBlock::setAdditionalState(const QJsonObject &state) {
    setFilePath(state["filePath"].toString());
    setRotation(state["rotation"].toDouble());
    setShadow(state["shadow"].toBool());
}
