#include "X32OscMonitorBlock.h"

#include "core/MainController.h"


X32OscMonitorBlock::X32OscMonitorBlock(MainController* controller, QString uid)
    : BlockBase(controller, uid)
{
    m_widthIsResizable = true;
    m_heightIsResizable = true;
}
