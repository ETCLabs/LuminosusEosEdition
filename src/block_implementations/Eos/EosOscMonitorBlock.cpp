#include "EosOscMonitorBlock.h"

#include "core/MainController.h"


EosOscMonitorBlock::EosOscMonitorBlock(MainController* controller, QString uid)
    : BlockBase(controller, uid)
{
    m_widthIsResizable = true;
    m_heightIsResizable = true;
}
