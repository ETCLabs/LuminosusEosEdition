#include "CueListHelperBlock.h"

#include "core/MainController.h"
#include "block_implementations/Theater/CueListBlock.h"


CueListHelperBlock::CueListHelperBlock(MainController* controller, QString uid)
    : BlockBase(controller, uid)
{
}

void CueListHelperBlock::clearLists() {
    std::vector<QPointer<BlockInterface>> blocks = m_controller->blockManager()->getCurrentBlocks();
    for (QPointer<BlockInterface>& block: blocks) {
        if (!block) continue;
        if (block->getBlockInfo().typeName == CueListBlock::info().typeName) {
            CueListBlock* cueList = qobject_cast<CueListBlock*>(block);
            cueList->pauseAndClear();
        }
    }
}

void CueListHelperBlock::clearBenches() {
    std::vector<QPointer<BlockInterface>> blocks = m_controller->blockManager()->getCurrentBlocks();
    for (QPointer<BlockInterface>& block: blocks) {
        if (!block) continue;
        if (block->isSceneBlock()) {
            block->clearBench();
        }
    }
}
