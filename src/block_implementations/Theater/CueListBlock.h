#ifndef CUELISTBLOCK_H
#define CUELISTBLOCK_H

#include "core/block_data/BlockBase.h"
#include "core/Cue.h"


class CueListBlock : public BlockBase
{
    Q_OBJECT

    Q_PROPERTY(QList<QObject*> cues READ getCues NOTIFY cuesChanged)
    Q_PROPERTY(QObject* lastCue READ getLastCue NOTIFY cueStatesChanged)
    Q_PROPERTY(QObject* activeCue READ getActiveCue NOTIFY cueStatesChanged)
    Q_PROPERTY(QObject* pendingCue READ getPendingCue NOTIFY cueStatesChanged)

public:

    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Cue List";
        info.category << "Theater";
        info.helpText = "";
        info.visibilityRequirements << VisibilityRequirement::StandaloneVersion;
        info.qmlFile = "qrc:/qml/Blocks/Theater/CueListBlock.qml";
        info.complete<CueListBlock>();
        return info;
    }

    explicit CueListBlock(MainController* controller, QString uid);

    virtual void getAdditionalState(QJsonObject& state) const override;
    virtual void setAdditionalState(const QJsonObject& state) override;

signals:
    void cuesChanged();
    void cueStatesChanged();

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    Cue* addSceneAsCue(PresetBlock* sceneBlock);

    void createCueFromMix();

    void go();

    void triggerRunning();

    void makePending(QObject* item);

    void removeCue(QObject* item);

    void moveUp(QObject* item);
    void moveDown(QObject* item);

    void moveAbove(QObject* cueToMoveO, QObject* anchorO);
    void moveBelow(QObject* cueToMoveO, QObject* anchorO);

    void pauseAndClear();

    void clearBenches();

    bool containsPreset(PresetBlock* sceneBlock) const;


    QList<QObject*> getCues();

    QObject* getLastCue() const { return m_lastCue; }
    QObject* getActiveCue() const { return m_activeCue; }
    QObject* getPendingCue() const { return m_pendingCue; }

private slots:
    void eachFrame(double timeSinceLastFrame);

    void convertPersistedCues();

    void onManualFadeChanged();

protected:
    void resetActiveCue();
    void chooseNewPendingCue();

    void sendInbetweenResult(const QMap<QPointer<BlockInterface>, HsvMatrix>& lastScene,
                             const QMap<QPointer<BlockInterface>, HsvMatrix>& activeScene,
                             double fadePos);

    void sendScene(const QMap<QPointer<BlockInterface>, HsvMatrix>& scene, double factor);

    void onEndOfHold();

protected:
    QPointer<NodeBase> m_goNode;
    QPointer<NodeBase> m_manualFadeNode;

    QList<QPointer<Cue>> m_cues;
    QJsonArray m_persistedCues;

    QPointer<Cue> m_lastCue;
    QPointer<Cue> m_activeCue;
    QPointer<Cue> m_pendingCue;

    BoolAttribute m_running;
    DoubleAttribute m_fadePos;
    DoubleAttribute m_holdPos;

    DoubleAttribute m_master;

    bool m_manualFadeUp;
};

#endif // CUELISTBLOCK_H
