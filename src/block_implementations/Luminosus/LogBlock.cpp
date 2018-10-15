#include "LogBlock.h"

#include "core/MainController.h"


LogBlock::LogBlock(MainController* controller, QString uid)
    : BlockBase(controller, uid)
{
    m_widthIsResizable = true;
    m_heightIsResizable = true;
}
