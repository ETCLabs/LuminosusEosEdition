#include "CueListBlock.h"

#include "core/MainController.h"
#include "core/Nodes.h"
#include "block_implementations/Theater/PresetBlock.h"


CueListBlock::CueListBlock(MainController* controller, QString uid)
    : BlockBase(controller, uid)
    , m_goNode(nullptr)
    , m_manualFadeNode(nullptr)
    , m_lastCue(nullptr)
    , m_activeCue(nullptr)
    , m_pendingCue(nullptr)
    , m_running(this, "running", false, /*persistent*/ false)
    , m_fadePos(this, "fadePos", 0.0, 0.0, 1.0, /*persistent*/ false)
    , m_holdPos(this, "holdPos", 0.0, 0.0, 1.0, /*persistent*/ false)
    , m_master(this, "master", 1.0)
    , m_manualFadeUp(true)
{
    m_widthIsResizable = true;
    m_heightIsResizable = true;

    m_goNode = createInputNode("go");
    m_goNode->enableImpulseDetection();
    connect(m_goNode, SIGNAL(impulseBegin()), this, SLOT(go()));

    m_manualFadeNode = createInputNode("manualFade");
    connect(m_manualFadeNode, SIGNAL(dataChanged()), this, SLOT(onManualFadeChanged()));

    connect(m_controller->projectManager(), SIGNAL(projectLoadingFinished()), this, SLOT(convertPersistedCues()));
    connect(m_controller->engine(), SIGNAL(updateBlocks(double)), this, SLOT(eachFrame(double)));
}

void CueListBlock::getAdditionalState(QJsonObject& state) const {
    QJsonArray arr;
    for (Cue* cue: m_cues) {
        if (!cue) continue;
        arr.append(cue->getState());
    }
    state["cues"] = arr;
}

void CueListBlock::setAdditionalState(const QJsonObject& state) {
    m_persistedCues = state["cues"].toArray();
    if (!m_controller->projectManager()->isLoading()) {
        convertPersistedCues();
    }
}

Cue* CueListBlock::addSceneAsCue(PresetBlock* sceneBlock) {
    Cue* cue = new Cue(m_controller, this, sceneBlock);
    m_cues.append(cue);
    if (!m_pendingCue) {
        m_pendingCue = cue;
    }
    emit cuesChanged();
    return cue;
}

void CueListBlock::createCueFromMix() {
    BlockInterface* block = m_controller->blockManager()->addNewBlock(PresetBlock::info().typeName);
    if (!block) {
        qWarning() << "CueListBlock::createCueFromMix(): Could not create Preset Block.";
        return;
    }
    PresetBlock* presetBlock = nullptr;
    if (block->getBlockInfo().typeName == PresetBlock::info().typeName) {
        presetBlock = qobject_cast<PresetBlock*>(block);
    }
    if (!presetBlock) return;
    presetBlock->saveFromMix();
    Cue* cue = addSceneAsCue(presetBlock);
    int number = m_cues.size();
    presetBlock->m_label.setValue(QString("Preset %1").arg(number));
    presetBlock->hideGui();
    if (getActiveCue()) {
        moveBelow(cue, getActiveCue());
    }
    makePending(cue);
}

void CueListBlock::go() {
    if (m_fadePos < 1.0 && m_activeCue) {
        // cue fade is running, skip to end:
        m_fadePos = 0.999;
    } else if (m_holdPos < 1.0 && m_activeCue && m_activeCue->follow()) {
        // hold is running, skip to end:
        m_holdPos = 0.999;
    } else {
        // cue fade and hold is at the end, fire next cue:
        m_lastCue = m_activeCue;
        if (!m_pendingCue) chooseNewPendingCue();
        m_activeCue = m_pendingCue;
        chooseNewPendingCue();
        emit cueStatesChanged();
        m_fadePos = 0.0;
        m_holdPos = 0.0;
    }
    m_running = true;
}

void CueListBlock::triggerRunning() {
    m_running = !m_running;
}

void CueListBlock::makePending(QObject* item) {
    Cue* cue = qobject_cast<Cue*>(item);
    if (!cue) return;
    if (!m_cues.contains(cue)) return;
    m_pendingCue = cue;
    emit cueStatesChanged();
}

void CueListBlock::removeCue(QObject* item) {
    Cue* cue = qobject_cast<Cue*>(item);
    if (!cue) return;
    if (!m_cues.contains(cue)) return;
    // remove scene data coming from this cue:
    if (cue->sceneBlock()) {
        QMap<QPointer<BlockInterface>, HsvMatrix>* scene = cue->sceneBlock()->getSceneData();
        if (scene) {
            auto end = scene->constEnd();
            for (auto it = scene->constBegin(); it != end; ++it) {
                const QPointer<BlockInterface>& block = it.key();
                if (!block) continue;
                block->removeSceneData(this);
            }
        }
    }
    m_cues.removeAll(cue);
    if (m_lastCue == cue) {
        m_lastCue = nullptr;
    }
    if (m_activeCue == cue) {
        resetActiveCue();
    }
    if (m_pendingCue == cue) {
        chooseNewPendingCue();
    }
    BlockInterface* sceneBlock = cue->sceneBlock();
    cue->deleteLater();
    if (sceneBlock && sceneBlock->guiShouldBeHidden()) {
        // the scene block of this cue was hidden and will be removed too
        // if it is used only in this cue:
        sceneBlock->deletedByUser();
    }
    emit cuesChanged();
}

void CueListBlock::moveUp(QObject* item) {
    Cue* cue = qobject_cast<Cue*>(item);
    if (!cue) return;
    if (!m_cues.contains(cue)) return;
    int index = m_cues.indexOf(cue);
    if (index > 0) {
        m_cues.removeAll(cue);
        m_cues.insert(index - 1, cue);
        emit cuesChanged();
    }
}

void CueListBlock::moveDown(QObject* item) {
    Cue* cue = qobject_cast<Cue*>(item);
    if (!cue) return;
    if (!m_cues.contains(cue)) return;
    int index = m_cues.indexOf(cue);
    if (index < m_cues.size() - 1) {
        m_cues.removeAll(cue);
        m_cues.insert(index + 1, cue);
        emit cuesChanged();
    }
}

void CueListBlock::moveAbove(QObject* cueToMoveO, QObject* anchorO) {
    Cue* cueToMove = qobject_cast<Cue*>(cueToMoveO);
    if (!cueToMove) return;
    if (!m_cues.contains(cueToMove)) return;

    Cue* anchor = qobject_cast<Cue*>(anchorO);
    if (!anchor) return;
    if (!m_cues.contains(anchor)) return;

    m_cues.removeAll(cueToMove);

    int anchorIndex = m_cues.indexOf(anchor);
    int newIndex = anchorIndex;
    if (newIndex < m_cues.size()) {
        m_cues.insert(newIndex, cueToMove);
    } else {
        m_cues.append(cueToMove);
    }
    emit cuesChanged();
}

void CueListBlock::moveBelow(QObject* cueToMoveO, QObject* anchorO) {
    Cue* cueToMove = qobject_cast<Cue*>(cueToMoveO);
    if (!cueToMove) return;
    if (!m_cues.contains(cueToMove)) return;

    Cue* anchor = qobject_cast<Cue*>(anchorO);
    if (!anchor) return;
    if (!m_cues.contains(anchor)) return;

    m_cues.removeAll(cueToMove);

    int anchorIndex = m_cues.indexOf(anchor);
    int newIndex = anchorIndex + 1;
    if (newIndex < m_cues.size()) {
        m_cues.insert(newIndex, cueToMove);
    } else {
        m_cues.append(cueToMove);
    }
    emit cuesChanged();
}

void CueListBlock::pauseAndClear() {
    m_running = false;
    if (m_lastCue && m_lastCue->sceneBlock()) {
        QMap<QPointer<BlockInterface>, HsvMatrix>* scene = m_lastCue->sceneBlock()->getSceneData();
        if (scene) {
            auto end = scene->constEnd();
            for (auto it = scene->constBegin(); it != end; ++it) {
                const QPointer<BlockInterface>& block = it.key();
                if (!block) continue;
                block->removeSceneData(this);
            }
        }
    }
    if (m_activeCue && m_activeCue->sceneBlock()) {
        QMap<QPointer<BlockInterface>, HsvMatrix>* scene = m_activeCue->sceneBlock()->getSceneData();
        if (scene) {
            auto end = scene->constEnd();
            for (auto it = scene->constBegin(); it != end; ++it) {
                const QPointer<BlockInterface>& block = it.key();
                if (!block) continue;
                block->removeSceneData(this);
            }
        }
    }
    m_lastCue = nullptr;
    m_pendingCue = m_activeCue;
    m_activeCue = nullptr;
    m_fadePos = 0.0;
    m_holdPos = 0.0;
    emit cueStatesChanged();
}

void CueListBlock::clearBenches() {
    std::vector<QPointer<BlockInterface>> blocks = m_controller->blockManager()->getCurrentBlocks();
    for (QPointer<BlockInterface>& block: blocks) {
        if (!block) continue;
        if (block->isSceneBlock()) {
            block->clearBench();
        }
    }
}

bool CueListBlock::containsPreset(PresetBlock* sceneBlock) const {
    for (Cue* cue: m_cues) {
        if (!cue) {
            qCritical() << "CueListBlock::containsPreset(): Cue was somehow deleted";
            continue;
        }
        if (cue->sceneBlock() == sceneBlock) {
            return true;
        }
    }
    return false;
}

QList<QObject*> CueListBlock::getCues() {
    QList<QObject*> cues;
    for (Cue* cue: m_cues) {
        if (!cue) {
            qCritical() << "CueListBlock::getCues(): Cue was somehow deleted";
            continue;
        }
        cues.append(static_cast<QObject*>(cue));
    }
    return cues;
}

void CueListBlock::eachFrame(double timeSinceLastFrame) {
    // prerequirements:
    if (!m_running) return;
    Cue* activeCue = m_activeCue;
    if (!activeCue) {
        qWarning() << "CueListBlock::eachFrame(): active cue is not set";
        m_running = false;
        return;
    }

    if (m_fadePos < 1.0) {
        // is still in fade phase:

        double fadeIn = activeCue->fadeIn();
        if (fadeIn <= (1.0 / 50.0)) {
            m_fadePos = 1.0;
        } else {
            m_fadePos = m_fadePos + timeSinceLastFrame / fadeIn;
        }
        if (m_fadePos < 1.0) {
            // in fade:
            Cue* lastCue = m_lastCue;
            if (!lastCue) {
                // last cue doesn't exist, fade from black:
                QMap<QPointer<BlockInterface>, HsvMatrix>* activeScene = nullptr;
                if (activeCue->sceneBlock()) {
                    activeScene = activeCue->sceneBlock()->getSceneData();
                }
                if (!activeScene) {
                    qWarning()<< "CueListBlock::eachFrame(): Scene data has been deleted 1.";
                    return;
                }
                sendScene(*activeScene, m_fadePos);
            } else {
                // last cue exists, fade from there:
                QMap<QPointer<BlockInterface>, HsvMatrix>* lastScene = nullptr;
                QMap<QPointer<BlockInterface>, HsvMatrix>* activeScene = nullptr;
                if (lastCue->sceneBlock()) {
                    lastScene = lastCue->sceneBlock()->getSceneData();
                }
                if (activeCue->sceneBlock()) {
                    activeScene = activeCue->sceneBlock()->getSceneData();
                }
                if (!lastScene || !activeScene) {
                    qWarning() << "CueListBlock::eachFrame(): Scene data has been deleted 2.";
                    return;
                }
                // real fading:
                sendInbetweenResult(*lastScene, *activeScene, m_fadePos);
            }
        } else {
            // end of fade
            if (m_lastCue && m_lastCue->sceneBlock()) {
                QMap<QPointer<BlockInterface>, HsvMatrix>* scene = m_lastCue->sceneBlock()->getSceneData();
                if (scene) {
                    auto end = scene->constEnd();
                    for (auto it = scene->constBegin(); it != end; ++it) {
                        const QPointer<BlockInterface>& block = it.key();
                        if (!block) continue;
                        block->removeSceneData(this);
                    }
                }
            }
            QMap<QPointer<BlockInterface>, HsvMatrix>* activeScene = nullptr;
            if (activeCue->sceneBlock()) {
                activeScene = activeCue->sceneBlock()->getSceneData();
            }
            if (!activeScene) {
                qWarning()<< "CueListBlock::eachFrame(): Scene data has been deleted 1.";
                return;
            }
            sendScene(*activeScene, 1.0);

            if (!activeCue->follow()) {
                m_running = false;
            }  // else -> hold phase will start in the nexct call of this method
            return;
        }
        return;

    } else if (m_holdPos < 1.0) {
        if (!activeCue->follow()) {
            // not a follow cue:
            m_holdPos = 1.0;
            return;
        }
        // hold phase:
        double holdTime = activeCue->hold();
        if (holdTime <= (1.0 / 50.0)) {
            // skip hold:
            m_holdPos = 1.0;
        } else {
            m_holdPos = m_holdPos + timeSinceLastFrame / holdTime;
        }
        if (m_holdPos < 1.0) {
            // in hold phase, wait...
            return;
        } else {
            // end of hold phase:
            onEndOfHold();
            return;
        }
        return;
    }
    // m_running is true, but fades are complete
    m_running = false;
}

void CueListBlock::resetActiveCue() {
    if (m_cues.isEmpty()) {
        m_activeCue = nullptr;
        return;
    }
    m_activeCue = m_cues.at(0);
}

void CueListBlock::chooseNewPendingCue() {
    if (!m_activeCue) {
        if (!m_cues.isEmpty()) {
            m_pendingCue = m_cues.at(0);
        }
        return;
    }
    int index = m_cues.indexOf(m_activeCue);
    if (index < 0) {
        // try to fix it:
        if (!m_cues.isEmpty()) {
            m_activeCue = m_cues.at(0);
            index = m_cues.indexOf(m_activeCue);
        }
        if (index < 0) {
            // could not fix it:
            qWarning() << "CueListBlock::eachFrame(): error, active cue not in cue list";
            return;
        }
    }
    if (index == m_cues.size() - 1) {
        // active cue is last cue
        m_pendingCue = m_cues.at(0);
    } else {
        m_pendingCue = m_cues.at(index + 1);
    }
}

void CueListBlock::sendInbetweenResult(const QMap<QPointer<BlockInterface>, HsvMatrix>& lastScene,
                                       const QMap<QPointer<BlockInterface>, HsvMatrix>& activeScene,
                                       double fadePos)
{
    double master = m_master;
    auto end = lastScene.constEnd();
    for (auto it = lastScene.constBegin(); it != end; ++it) {
        if (activeScene.contains(it.key())) {
            // block is in last and active scene, calculate inbetween result:
            const QPointer<BlockInterface>& block = it.key();
            if (!block) continue;
            HsvMatrix lastMatCopy = it.value();
            const HsvMatrix& activeMat = activeScene.value(it.key());
            lastMatCopy.fadeTo(activeMat, fadePos);

            block->setSceneData(this, master, lastMatCopy);

        } else {
            // block is not anymore in active scene, fade out:
            const QPointer<BlockInterface>& block = it.key();
            if (!block) continue;
            block->setSceneData(this, (1 - fadePos) * master, it.value());
        }
    }
    auto endActive = activeScene.constEnd();
    for (auto it = activeScene.constBegin(); it != endActive; ++it) {
        if (!lastScene.contains(it.key())) {
            // block is new in active scene, fade in:
            const QPointer<BlockInterface>& block = it.key();
            if (!block) continue;
            block->setSceneData(this, fadePos * master, it.value());
        }
    }
}

void CueListBlock::sendScene(const QMap<QPointer<BlockInterface>, HsvMatrix>& scene, double factor) {
    double master = m_master;
    auto end = scene.constEnd();
    for (auto it = scene.constBegin(); it != end; ++it) {
        const QPointer<BlockInterface>& block = it.key();
        if (!block) continue;
        block->setSceneData(this, factor * master, it.value());
    }
}

void CueListBlock::onEndOfHold() {
    m_lastCue = m_activeCue;
    if (!m_pendingCue) chooseNewPendingCue();
    m_activeCue = m_pendingCue;
    chooseNewPendingCue();
    m_fadePos = 0.0;
    m_holdPos = 0.0;
    m_running = true;
    emit cueStatesChanged();
}

void CueListBlock::convertPersistedCues() {
    m_cues.clear();
    for (QJsonValueRef cueState: m_persistedCues) {
        Cue* cue = new Cue(m_controller, this, nullptr);
        cue->setState(cueState.toObject());
        m_cues.append(cue);
    }
    if (!m_cues.isEmpty()) {
        m_pendingCue = m_cues.at(0);
    }
    emit cuesChanged();
}

void CueListBlock::onManualFadeChanged() {
    if (!m_activeCue) {
        if (!m_pendingCue) chooseNewPendingCue();
        m_activeCue = m_pendingCue;
        m_fadePos = 0.0;
        m_holdPos = 0.0;
        chooseNewPendingCue();
        emit cueStatesChanged();
    }
    Cue* activeCue = m_activeCue;
    if (!activeCue) return;
    double val = m_manualFadeNode->getValue();

    // apply dead zone to fix problems with inaccruate MIDI controllers:
    const double deadzone = 0.05;
    val = limit(0.0, (val - deadzone) / (1.0 - 2*deadzone), 1.0);

    if (m_fadePos >= 1.0) {
        // go to next cue:
        m_lastCue = m_activeCue;
        if (!m_pendingCue) chooseNewPendingCue();
        m_activeCue = m_pendingCue;
        chooseNewPendingCue();
        emit cueStatesChanged();
    }

    m_fadePos = m_manualFadeUp ? val : (1.0 - val);

    if (m_fadePos < 1.0) {
        Cue* lastCue = m_lastCue;
        if (!lastCue) {
            // last cue doesn't exist, fade from black:
            QMap<QPointer<BlockInterface>, HsvMatrix>* activeScene = nullptr;
            if (activeCue->sceneBlock()) {
                activeScene = activeCue->sceneBlock()->getSceneData();
            }
            if (!activeScene) {
                qWarning()<< "CueListBlock::eachFrame(): Scene data has been deleted 1.";
                return;
            }
            sendScene(*activeScene, m_fadePos);
        } else {
            // last cue exists, fade from there:
            QMap<QPointer<BlockInterface>, HsvMatrix>* lastScene = nullptr;
            QMap<QPointer<BlockInterface>, HsvMatrix>* activeScene = nullptr;
            if (lastCue->sceneBlock()) {
                lastScene = lastCue->sceneBlock()->getSceneData();
            }
            if (activeCue->sceneBlock()) {
                activeScene = activeCue->sceneBlock()->getSceneData();
            }
            if (!lastScene || !activeScene) {
                qWarning() << "CueListBlock::eachFrame(): Scene data has been deleted 2.";
                return;
            }
            // real fading:
            sendInbetweenResult(*lastScene, *activeScene, m_fadePos);
        }
    } else {
        // end of fade
        if (m_lastCue && m_lastCue->sceneBlock()) {
            QMap<QPointer<BlockInterface>, HsvMatrix>* scene = m_lastCue->sceneBlock()->getSceneData();
            if (scene) {
                auto end = scene->constEnd();
                for (auto it = scene->constBegin(); it != end; ++it) {
                    const QPointer<BlockInterface>& block = it.key();
                    if (!block) continue;
                    block->removeSceneData(this);
                }
            }
        }
        QMap<QPointer<BlockInterface>, HsvMatrix>* activeScene = nullptr;
        if (activeCue->sceneBlock()) {
            activeScene = activeCue->sceneBlock()->getSceneData();
        }
        if (!activeScene) {
            qWarning()<< "CueListBlock::eachFrame(): Scene data has been deleted 1.";
            return;
        }
        sendScene(*activeScene, 1.0);

        m_manualFadeUp = !m_manualFadeUp;
    }
}
