#include "RecorderSlaveBlock.h"

#include "core/MainController.h"
#include "core/Nodes.h"


RecorderSlaveBlock::RecorderSlaveBlock(MainController* controller, QString uid)
    : InOutBlock(controller, uid)
    , m_linkNode(nullptr)
    , m_recording(this, "recording", false, /*persistent*/ false)
    , m_playing(this, "playing", false, /*persistent*/ false)
    , m_waitingForRecord(this, "waitingForRecord", false, /*persistent*/ false)
    , m_playbackPosition(0)
    , m_data()
{
    m_linkNode = createInputNode("link");

    m_linkNode->enableImpulseDetection();
    connect(m_linkNode, SIGNAL(impulseBegin()), this, SLOT(startAtBegin()));

    connect(m_controller->engine(), SIGNAL(updateBlocks(double)), this, SLOT(eachFrame()));
}

void RecorderSlaveBlock::getAdditionalState(QJsonObject& state) const {
    state["data"] = serialize<QVector<double>>(m_data);
}

void RecorderSlaveBlock::setAdditionalState(const QJsonObject& state) {
    readAttributesFrom(state);
    m_data = deserialize<QVector<double>>(state["data"].toString());
}

void RecorderSlaveBlock::startRecording() {
    m_data.clear();
    m_recording.setValue(true);
}

void RecorderSlaveBlock::stopRecording() {
    m_recording.setValue(false);
}

void RecorderSlaveBlock::sync() {
    m_playbackPosition = 0;
}

void RecorderSlaveBlock::startAtBegin() {
    if (m_recording) {
        stopRecording();
    }
    m_playbackPosition = 0;
    m_playing.setValue(true);

    if (m_waitingForRecord) {
        m_waitingForRecord.setValue(false);
        startRecording();
    }
}

void RecorderSlaveBlock::toggleOverdub() {
    m_waitingForRecord.setValue(!m_waitingForRecord);
}

void RecorderSlaveBlock::eachFrame() {
    if (m_recording) {
        // recording
        if (m_linkNode->getValue() < LuminosusConstants::triggerThreshold) {
            stopRecording();
            return;
        }

        double value = m_inputNode->getValue();
        m_data.append(value);
        m_outputNode->setValue(value);
        emit dataChanged();
        return;
    }

    if (m_playing && !m_data.isEmpty()) {
        // playing
        double value = m_data[m_playbackPosition % m_data.size()];
        m_outputNode->setValue(value);

        // either progress frame or stop:
        if (m_playbackPosition + 1 >= m_data.size() || m_linkNode->getValue() < LuminosusConstants::triggerThreshold) {
            m_playing.setValue(false);
            m_playbackPosition = 0;
            emit playbackPositionChanged();
        } else {
            m_playbackPosition = (m_playbackPosition + 1) % m_data.size();
            emit playbackPositionChanged();
        }
    }
}
