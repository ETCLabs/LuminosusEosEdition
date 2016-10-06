#include "PageAnchorBlock.h"

#include "MainController.h"


PageAnchorBlock::PageAnchorBlock(MainController* controller, QString uid)
    : BlockBase(controller, uid, info().qmlFile)
{
    m_controller->anchorManager()->addAnchor(this);

    connect(m_controller->anchorManager(), SIGNAL(anchorsChanged()), this, SIGNAL(anchorIndexChanged()));
}

PageAnchorBlock::~PageAnchorBlock() {
    m_controller->anchorManager()->removeAnchor(this);
}

QJsonObject PageAnchorBlock::getState() const {
    QJsonObject state;
    return state;
}

void PageAnchorBlock::setState(const QJsonObject& state) {

}

int PageAnchorBlock::getAnchorIndex() {
    return m_controller->anchorManager()->getAnchorIndex(this);
}
