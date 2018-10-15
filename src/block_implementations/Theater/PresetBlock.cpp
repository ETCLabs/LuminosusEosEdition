#include "PresetBlock.h"

#include "core/MainController.h"
#include "CueListBlock.h"  // for typeName

PresetBlock::PresetBlock(MainController* controller, QString uid)
    : BlockBase(controller, uid)
    , m_value(this, "value", 0.0)
{
    m_heightIsResizable = true;

    connect(m_controller->projectManager(), SIGNAL(projectLoadingFinished()), this, SLOT(convertPersistentSceneData()));
    connect(&m_value, SIGNAL(valueChanged()), this, SLOT(update()));

    if (m_guiItem) {
        connect(m_guiItem, SIGNAL(parentChanged(QQuickItem*)), this, SIGNAL(blockIsVisibleChanged()));
    }

    connect(this, SIGNAL(guiIsHiddenChanged()), this, SLOT(releaseIfHidden()));
}

PresetBlock::~PresetBlock() {
    // remove scene data:
    auto end = m_sceneData.constEnd();
    for (auto it = m_sceneData.constBegin(); it != end; ++it) {
        QPointer<BlockInterface> block = it.key();
        if (!block) continue;
        block->removeSceneData(this);
    }
}

void PresetBlock::getAdditionalState(QJsonObject& state) const {
    QMap<QString, HsvMatrix> persistentSceneData;
    auto end = m_sceneData.constEnd();
    for (auto it = m_sceneData.constBegin(); it != end; ++it) {
        QPointer<BlockInterface> block = it.key();
        if (!block) continue;
        persistentSceneData[block->getUid()] = it.value();
    }

    state["sceneData"] = serialize(persistentSceneData);
}

void PresetBlock::setAdditionalState(const QJsonObject& state) {
    m_persistentSceneData = deserialize<QMap<QString, HsvMatrix>>(state["sceneData"].toString());
    if (!m_controller->projectManager()->isLoading()) {
        convertPersistentSceneData();
    }
}

bool PresetBlock::mayBeRemoved() {
    bool used = false;
    std::vector<QPointer<BlockInterface>> blocks = m_controller->blockManager()->getCurrentBlocks();
    for (QPointer<BlockInterface>& block: blocks) {
        if (!block) continue;
        if (block->getBlockInfo().typeName == CueListBlock::info().typeName) {
            CueListBlock* cueList = qobject_cast<CueListBlock*>(block);
            if (cueList->containsPreset(this)) {
                used = true;
                break;
            }
        }
    }
    if (used) {
        hideGui();
    }
    return !used;
}

void PresetBlock::saveFromMix() {
    m_sceneData.clear();
    std::vector<QPointer<BlockInterface>> blocks = m_controller->blockManager()->getCurrentBlocks();
    for (QPointer<BlockInterface>& block: blocks) {
        if (!block) continue;
        if (block->isSceneBlock()) {
            m_sceneData[block] = block->getMixData();
        }
    }
}

void PresetBlock::saveFromBenches() {
    m_sceneData.clear();
    std::vector<QPointer<BlockInterface>> blocks = m_controller->blockManager()->getCurrentBlocks();
    for (QPointer<BlockInterface>& block: blocks) {
        if (!block) continue;
        if (block->isSceneBlock()) {
            m_sceneData[block] = block->getBenchData();
            block->clearBench();
        }
    }

    if (m_value < 1.0) {
        m_value = 1.0;
        // -> triggers update()
    } else {
        update();
    }
}

void PresetBlock::pushToBanks() {
    double factor = m_value;
    if (factor <= 0.0) factor = 1.0;

    auto end = m_sceneData.constEnd();
    for (auto it = m_sceneData.constBegin(); it != end; ++it) {
        QPointer<BlockInterface> block = it.key();
        if (!block) continue;
        block->addToBench({factor, it.value()});
    }

    m_value = 0.0;
    // -> triggers update()
}

void PresetBlock::addToCueList() {
    std::vector<QPointer<BlockInterface>> blocks = m_controller->blockManager()->getCurrentBlocks();
    for (QPointer<BlockInterface>& block: blocks) {
        if (!block) continue;
        if (block->getBlockInfo().typeName == CueListBlock::info().typeName) {
            CueListBlock* cueList = qobject_cast<CueListBlock*>(block);
            cueList->addSceneAsCue(this);
        }
    }
}

void PresetBlock::releaseIfHidden() {
    if (guiShouldBeHidden() && m_value) {
        m_value = 0.0;
    }
}

void PresetBlock::update() {
    double factor = m_value;

    if (factor > 0) {
        auto end = m_sceneData.constEnd();
        for (auto it = m_sceneData.constBegin(); it != end; ++it) {
            QPointer<BlockInterface> block = it.key();
            if (!block) continue;
            block->setSceneData(this, factor, it.value());
        }
    } else {
        auto end = m_sceneData.constEnd();
        for (auto it = m_sceneData.constBegin(); it != end; ++it) {
            QPointer<BlockInterface> block = it.key();
            if (!block) continue;
            block->removeSceneData(this);
        }
    }
}

void PresetBlock::convertPersistentSceneData() {
    m_sceneData.clear();
    auto end = m_persistentSceneData.constEnd();
    for (auto it = m_persistentSceneData.constBegin(); it != end; ++it) {
        BlockInterface* block = m_controller->blockManager()->getBlockByUid(it.key());
        if (!block) continue;
        m_sceneData[block] = it.value();
    }
    update();
}
