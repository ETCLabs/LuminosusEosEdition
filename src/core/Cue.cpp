#include "core/Cue.h"

#include "core/MainController.h"
#include "block_implementations/Theater/PresetBlock.h"


Cue::Cue(MainController* controller, BlockInterface* block, PresetBlock* sceneBlock)
    : QObject(controller)
    , m_controller(controller)
    , m_block(block)
    , m_sceneBlock(sceneBlock)
    , m_description("")
    , m_fadeIn(1.5)
    , m_follow(false)
    , m_hold(2.5)
{

}

QJsonObject Cue::getState() const {
    QJsonObject state;
    if (m_sceneBlock) {
        state["sceneBlock"] = m_sceneBlock->getUid();
    }
    state["description"] = description();
    state["fadeIn"] = fadeIn();
    state["follow"] = follow();
    state["hold"] = hold();
    return state;
}

void Cue::setState(const QJsonObject& state) {
    QString sceneBlockUid = state["sceneBlock"].toString();
    BlockInterface* sceneBlock = m_controller->blockManager()->getBlockByUid(sceneBlockUid);
    if (sceneBlock && sceneBlock->getBlockInfo().typeName == PresetBlock::info().typeName) {
        m_sceneBlock = qobject_cast<PresetBlock*>(sceneBlock);
    }
    if (!m_sceneBlock) {
        qCritical() << "Cue: could not find Scene Block instance.";
    }
    setDescription(state["description"].toString());
    setFadeIn(state["fadeIn"].toDouble());
    setFollow(state["follow"].toBool());
    setHold(state["hold"].toDouble());
}

PresetBlock* Cue::sceneBlock() const { return m_sceneBlock; }

void Cue::setSceneBlock(PresetBlock* sceneBlock) { m_sceneBlock = sceneBlock; emit sceneBlockChanged(); }
