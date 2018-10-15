#include "RecorderBlock.h"

#include "core/MainController.h"
#include "core/Nodes.h"


RecorderBlock::RecorderBlock(MainController* controller, QString uid)
    : InOutBlock(controller, uid)
    , m_recordNode(nullptr)
    , m_playNode(nullptr)
    , m_pauseNode(nullptr)
    , m_toggleNode(nullptr)
    , m_linkNode(nullptr)
    , m_recording(this, "recording", false, /*persistent*/ false)
    , m_playing(this, "playing", false, /*persistent*/ true)
    , m_loop(this, "loop", true)
    , m_playAfterRecord(this, "playAfterRecord", true)
    , m_waitingForOverdub(this, "waitingForOverdub", false, /*persistent*/ false)
    , m_playbackPosition(0)
    , m_data()
{
    m_recordNode = createInputNode("record");
    m_playNode = createInputNode("play");
    m_pauseNode = createInputNode("pause");
    m_toggleNode = createInputNode("toggle");
    m_linkNode = createOutputNode("link");

    m_recordNode->enableImpulseDetection();
    connect(m_recordNode, SIGNAL(impulseBegin()), this, SLOT(startRecording()));
    connect(m_recordNode, SIGNAL(impulseEnd()), this, SLOT(stopRecording()));

    m_playNode->enableImpulseDetection();
    connect(m_playNode, SIGNAL(impulseBegin()), this, SLOT(startPlayback()));

    m_pauseNode->enableImpulseDetection();
    connect(m_pauseNode, SIGNAL(impulseBegin()), this, SLOT(stopPlayback()));

    m_toggleNode->enableImpulseDetection();
    connect(m_toggleNode, SIGNAL(impulseBegin()), this, SLOT(startPlayback()));
    connect(m_toggleNode, SIGNAL(impulseEnd()), this, SLOT(stopPlayback()));

    connect(m_controller->engine(), SIGNAL(updateBlocks(double)), this, SLOT(eachFrame()));
}

void RecorderBlock::getAdditionalState(QJsonObject& state) const {
    state["data"] = serialize<QVector<double>>(m_data);
}

void RecorderBlock::setAdditionalState(const QJsonObject& state) {
    readAttributesFrom(state);
    m_data = deserialize<QVector<double>>(state["data"].toString());
}

void RecorderBlock::startRecording() {
    if (m_playing) {
        stopPlayback();
    }
    m_data.clear();
    m_recording.setValue(true);
}

void RecorderBlock::stopRecording() {
    m_recording.setValue(false);
    if (m_playAfterRecord) {
        startPlayback();
    }
}

void RecorderBlock::startPlayback() {
    if (m_recording) {
        stopRecording();
    }
    m_playing.setValue(true);
}

void RecorderBlock::stopPlayback() {
    m_playing.setValue(false);
    m_linkNode->setValue(0.0);
    m_playbackPosition = 0;
    emit playbackPositionChanged();
}

void RecorderBlock::sync() {
    m_playbackPosition = 0;
    m_linkNode->setValue(0.0);
}

void RecorderBlock::toggleOverdub() {
    m_waitingForOverdub.setValue(!m_waitingForOverdub);
}

double RecorderBlock::getBpm() const {
    double duration = getDuration();
    if (duration <= 0) return 0.0;
    double bpm = 60.0 / duration;

    if (bpm < 75) {
        while (bpm < 75) {
            bpm *= 2;
        }
    } else {
        while (bpm > 150) {
            bpm /= 2.0;
        }
    }
    return bpm;
}

void RecorderBlock::eachFrame() {
    if (m_playing && !m_data.isEmpty()) {
        // playing
        double value = m_data[m_playbackPosition % m_data.size()];
        m_outputNode->setValue(value);

        // link logic:
        if (m_linkNode->getValue() < LuminosusConstants::triggerThreshold) {
            m_linkNode->setValue(1.0);
        } else if (m_playbackPosition + 1 >= m_data.size()) {
            // this is the last frame
            m_linkNode->setValue(0.0);
        }

        // either progress frame or stop:
        if (m_playbackPosition + 1 >= m_data.size() && !m_loop) {
            stopPlayback();
        } else {
            m_playbackPosition = (m_playbackPosition + 1) % m_data.size();
            emit playbackPositionChanged();
        }
    }

    if (m_recording) {
        // recording
        double value = m_inputNode->getValue();
        m_data.append(value);
        m_outputNode->setValue(value);
        emit dataChanged();
        emit durationChanged();
    }
}


