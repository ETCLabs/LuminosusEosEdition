#ifndef RECORDERBLOCK_H
#define RECORDERBLOCK_H

#include "core/block_data/InOutBlock.h"
#include "core/SmartAttribute.h"
#include "core/Matrix.h"
#include "core/Nodes.h"
#include "utils.h"


class RecorderBlock : public InOutBlock
{
    Q_OBJECT

    Q_PROPERTY(QVector<double> data READ getData NOTIFY dataChanged)
    Q_PROPERTY(double relativePosition READ getRelativePosition NOTIFY playbackPositionChanged)
    Q_PROPERTY(double duration READ getDuration NOTIFY durationChanged)
    Q_PROPERTY(double bpm READ getBpm NOTIFY durationChanged)

public:

    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Recorder";
        info.nameInUi = "Recorder";
        info.category << "General" << "Logic";
        info.helpText = "";
        info.visibilityRequirements << VisibilityRequirement::DeveloperMode;
        info.qmlFile = "qrc:/qml/Blocks/Logic/RecorderBlock.qml";
        info.complete<RecorderBlock>();
        return info;
    }

    explicit RecorderBlock(MainController* controller, QString uid);

    void getAdditionalState(QJsonObject& state) const override;
    void setAdditionalState(const QJsonObject& state) override;

signals:
    void dataChanged();
    void playbackPositionChanged();
    void durationChanged();

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    void startRecording();

    void stopRecording();

    void startPlayback();

    void stopPlayback();

    void sync();

    void toggleOverdub();

    // ------------ Getter + Setter --------------

    QVector<double> getData() const { return m_data; }

    double getRelativePosition() const { return m_data.size() > 0 ? double(m_playbackPosition) / m_data.size() : 0.0; }

    double getDuration() const { return m_data.size() / 50.0 /* 50 FPS */; }

    double getBpm() const;


private slots:
    void eachFrame();

protected:
    QPointer<NodeBase> m_recordNode;
    QPointer<NodeBase> m_playNode;
    QPointer<NodeBase> m_pauseNode;
    QPointer<NodeBase> m_toggleNode;
    QPointer<NodeBase> m_linkNode;

    BoolAttribute m_recording;
    BoolAttribute m_playing;
    BoolAttribute m_loop;
    BoolAttribute m_playAfterRecord;
    BoolAttribute m_waitingForOverdub;

    bool m_overdubbing;

    int m_playbackPosition;

    QVector<double> m_data;
};

#endif // RECORDERBLOCK_H
