#include "VideoBlock.h"

#include "core/MainController.h"


VideoBlock::VideoBlock(MainController* controller, QString uid)
    : BlockBase(controller, uid)
    , m_filePath(this, "filePath", "")
{
    m_widthIsResizable = true;
}
