#include "PageAnchorBlock.h"

#include "core/MainController.h"


PageAnchorBlock::PageAnchorBlock(MainController* controller, QString uid)
    : BlockBase(controller, uid)
{
    m_controller->anchorManager()->addAnchor(this);

    connect(m_controller->anchorManager(), SIGNAL(anchorsChanged()), this, SIGNAL(anchorIndexChanged()));
}

PageAnchorBlock::~PageAnchorBlock() {
    m_controller->anchorManager()->removeAnchor(this);
}

int PageAnchorBlock::getAnchorIndex() {
    return m_controller->anchorManager()->getAnchorIndex(this);
}
