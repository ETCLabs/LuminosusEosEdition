#include "SequencerBlock.h"

#include "core/MainController.h"
#include "core/Nodes.h"


SequencerBlock::SequencerBlock(MainController* controller, QString uid)
    : BlockBase(controller, uid)
    , m_time(this, "time", 3.0, 0.01, 3600)
    , m_fade(this, "fade", 0.5)
    , m_pos(0.0)
    , m_fadePos(0.0)
    , m_stepNumber(0)
    , m_holdPartSet(false)
{
    m_outputNode = createOutputNode("outputNode");
    m_timeNode = createInputNode("time");
    connect(m_timeNode, &NodeBase::dataChanged, [this](){ m_time.setValue(m_timeNode->getAbsoluteValue()); });
    connect(m_controller->engine(), SIGNAL(updateBlocks(double)), this, SLOT(eachFrame(double)));
    connect(m_outputNode, SIGNAL(requestedSizeChanged()), this, SLOT(updateRequestedSize()));
    addNode();
    addNode();
}

void SequencerBlock::getAdditionalState(QJsonObject& state) const {
    state["dynamicNodesCount"] = m_dynamicNodeNames.size();
}

void SequencerBlock::setAdditionalState(const QJsonObject &state) {
    readAttributesFrom(state);
    int dynNodesCount = state["dynamicNodesCount"].toInt();
    while (!m_dynamicNodeNames.isEmpty()) removeDynamicNode(true);
    for (int i=0; i < dynNodesCount; ++i) {
        addNode();
    }
}

void SequencerBlock::addNode() {
    int num = m_dynamicNodeNames.size();
    QString name = QString("dynamicNode") + QString::number(num);
    NodeBase* node = createInputNode(name);
    m_dynamicNodes.append(node);
    m_dynamicNodeNames.append(name);
    emit dynamicNodesChanged();
}

void SequencerBlock::removeDynamicNode(bool forced) {
    if (m_dynamicNodeNames.size() <= 2 && !forced) return;
    if (m_dynamicNodeNames.size() <= 0) return;
    NodeBase* node = m_dynamicNodes.last();
    if (!node) {
        qCritical() << "Sequencer removeNode: could not get last node.";
        return;
    }
    m_dynamicNodes.removeLast();
    m_dynamicNodeNames.removeLast();
    removeNode(node);
    emit dynamicNodesChanged();
}

void SequencerBlock::sync() {
    m_pos = 0.0;
    m_fadePos = 0.0;
    m_stepNumber = 0;
    m_holdPartSet = false;
    emit stepNumberChanged();
    emit fadePositionChanged();
}

void SequencerBlock::eachFrame(double timeSinceLastFrame) {
    if (m_time <= 0) return;
    if (m_dynamicNodes.size() < 2) return;
    double progress = timeSinceLastFrame / m_time;
    m_pos = m_pos + progress;
    if (m_pos > 1) {
        // next step
        m_pos = fmod(m_pos, 1.0);
        m_fadePos = 0.0;
        m_stepNumber = (m_stepNumber + 1) % m_dynamicNodes.size();
        m_holdPartSet = false;
        emit stepNumberChanged();
    }

    if (m_pos < (1 - m_fade)) {
        // hold part
        if(!m_holdPartSet) {
            const ColorMatrix& input = m_dynamicNodes[m_stepNumber % m_dynamicNodes.size()]->constData();
            auto out = RgbDataModifier(m_outputNode);
            if (input.width() < out.width || input.height() < out.height) {
                qWarning() << "Sequencer: Input too small.";
                return;
            }

            for (int x = 0; x < out.width; ++x) {
                for (int y = 0; y < out.height; ++y) {
                    out.set(x, y, input.getRgbAt(x, y));
                }
            }
            m_holdPartSet = false;  // always update to allow effects pass through
        }

    } else {
        // fade part
        if (m_fade <= 0) return;
        m_fadePos = (m_pos - (1 - m_fade)) / m_fade;
        double ratio = m_fadePos;
        const ColorMatrix& input1 = m_dynamicNodes[m_stepNumber % m_dynamicNodes.size()]->constData();
        const ColorMatrix& input2 = m_dynamicNodes[(m_stepNumber+1) % m_dynamicNodes.size()]->constData();
        auto out = RgbDataModifier(m_outputNode);
        if (input1.width() < out.width || input1.height() < out.height
                || input2.width() < out.width || input2.height() < out.height) {
            qWarning() << "Sequencer: Input too small.";
            return;
        }

        for (int x = 0; x < out.width; ++x) {
            for (int y = 0; y < out.height; ++y) {
                RGB mix = input1.getRgbAt(x, y) * (1 - ratio);
                mix = mix + input2.getRgbAt(x, y) * ratio;
                out.set(x, y, mix);
            }
        }
        emit fadePositionChanged();
    }
}

void SequencerBlock::updateRequestedSize() {
    Size s = m_outputNode->getRequestedSize();
    for (NodeBase* node: m_dynamicNodes) {
        node->setRequestedSize(s);
    }
}
