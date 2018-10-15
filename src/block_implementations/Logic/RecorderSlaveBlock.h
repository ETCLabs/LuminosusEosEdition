#ifndef RECORDERSLAVEBLOCK_H
#define RECORDERSLAVEBLOCK_H

#include "core/block_data/InOutBlock.h"
#include "core/SmartAttribute.h"
#include "core/Matrix.h"
#include "core/Nodes.h"
#include "utils.h"


class RecorderSlaveBlock : public InOutBlock
{
    Q_OBJECT

    Q_PROPERTY(QVector<double> data READ getData NOTIFY dataChanged)
    Q_PROPERTY(double relativePosition READ getRelativePosition NOTIFY playbackPositionChanged)

public:

    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Recorder Slave";
        info.nameInUi = "Recorder Extension";
        info.category << "General" << "Logic";
        info.helpText = "";
        info.visibilityRequirements << VisibilityRequirement::DeveloperMode;
        info.qmlFile = "qrc:/qml/Blocks/Logic/RecorderSlaveBlock.qml";
        info.complete<RecorderSlaveBlock>();
        return info;
    }

    explicit RecorderSlaveBlock(MainController* controller, QString uid);

    void getAdditionalState(QJsonObject& state) const override;
    void setAdditionalState(const QJsonObject& state) override;

signals:
    void dataChanged();
    void playbackPositionChanged();

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    void startRecording();

    void stopRecording();

    void sync();

    void startAtBegin();

    void toggleOverdub();

    // ------------ Getter + Setter --------------

    QVector<double> getData() const { return m_data; }

    double getRelativePosition() const { return m_data.size() > 0 ? double(m_playbackPosition) / m_data.size() : 0.0; }


private slots:
    void eachFrame();

protected:
    QPointer<NodeBase> m_linkNode;

    BoolAttribute m_recording;
    BoolAttribute m_playing;
    BoolAttribute m_waitingForRecord;

    int m_playbackPosition;

    QVector<double> m_data;
};

#endif // RECORDERSLAVEBLOCK_H
