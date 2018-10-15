#include "CommandlineBlock.h"

#include "core/MainController.h"
#include "core/Nodes.h"

#include <QProcess>


CommandlineBlock::CommandlineBlock(MainController* controller, QString uid)
    : OneInputBlock(controller, uid)
    , m_command(this, "command")
{
    m_inputNode->enableImpulseDetection();
    connect(m_inputNode, SIGNAL(impulseBegin()), this, SLOT(execute()));
}

void CommandlineBlock::execute() {
    if (m_command.getValue().isEmpty()) return;
#ifdef Q_OS_LINUX
    QProcess::startDetached(m_command);
#endif
}
