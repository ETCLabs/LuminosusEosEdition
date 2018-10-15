#include "MovingPatternBlock.h"

#include "core/Nodes.h"


MovingPatternBlock::MovingPatternBlock(MainController* controller, QString uid)
    : InOutBlock(controller, uid)
    , m_amount(this, "amount", 0.2)
{
    connect(m_inputNode, SIGNAL(dataChanged()), this, SLOT(update()));
}

void MovingPatternBlock::update() {

}
