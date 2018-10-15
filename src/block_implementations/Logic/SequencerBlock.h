#ifndef SEQUENCERBLOCK_H
#define SEQUENCERBLOCK_H

#include "core/block_data/BlockBase.h"
#include "core/SmartAttribute.h"
#include "utils.h"


class SequencerBlock : public BlockBase
{
    Q_OBJECT

    Q_PROPERTY(QStringList dynamicNodeNames READ getDynamicNodeNames NOTIFY dynamicNodesChanged)
    Q_PROPERTY(int dynamicNodeCount READ getDynamicNodeCount NOTIFY dynamicNodesChanged)
    Q_PROPERTY(double fadePosition READ getFadePosition NOTIFY fadePositionChanged)
    Q_PROPERTY(int stepNumber READ getStepNumber NOTIFY stepNumberChanged)

public:

    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Sequencer";
        info.category << "General" << "Logic";
        info.helpText = "Shows two or more steps in a sequence with a given step time.\n\n"
                        "Fade Ratio: the part of the step time that is used to fade from "
                        "the previous step.\n\n"
                        "Tip: The step time could be connected to a BPM Detection block to "
                        "show a new step on every beat.";
        info.qmlFile = "qrc:/qml/Blocks/Logic/SequencerBlock.qml";
        info.complete<SequencerBlock>();
        return info;
    }

    explicit SequencerBlock(MainController* controller, QString uid);

    virtual void getAdditionalState(QJsonObject& state) const override;
    virtual void setAdditionalState(const QJsonObject& state) override;

signals:
    void dynamicNodesChanged();
    void stepNumberChanged();
    void fadePositionChanged();

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    void addNode();

    void removeDynamicNode(bool forced=false);

    void sync();

    QStringList getDynamicNodeNames() const { return m_dynamicNodeNames.toList(); }
    int getDynamicNodeCount() const { return m_dynamicNodes.size(); }

    double getFadePosition() const { return m_fadePos; }
    int getStepNumber() const { return m_stepNumber; }

private slots:
    void eachFrame(double timeSinceLastFrame);
    void updateRequestedSize();

protected:
    DoubleAttribute m_time;
    DoubleAttribute m_fade;

    QVector<QPointer<NodeBase>> m_dynamicNodes;
    QVector<QString> m_dynamicNodeNames;

    double m_pos;
    double m_fadePos;
    int m_stepNumber;
    bool m_holdPartSet;
    QPointer<NodeBase> m_outputNode;
    QPointer<NodeBase> m_timeNode;

};

#endif // SEQUENCERBLOCK_H
