#include "AudioPlaybackBlock.h"

#include "core/MainController.h"
#include "core/Nodes.h"

#include <QFileInfo>


AudioPlaybackBlock::AudioPlaybackBlock(MainController* controller, QString uid)
    : BlockBase(controller, uid)
    , m_playNode(nullptr)
    , m_pauseNode(nullptr)
    , m_toggleNode(nullptr)
    , m_activeNode(nullptr)
    , m_endNode(nullptr)
    , m_positionNode(nullptr)
    , m_filePath("")
    , m_lastPlayNodeValue(0.0)
    , m_lastPauseNodeValue(0.0)
    , m_lastToggleNodeValue(0.0)
    , m_alwaysStartAtBegin(false)
    , m_loop(false)
    , m_toggleMode(true)
    , m_player(this)
{
    m_widthIsResizable = true;

    m_playNode = createInputNode("playNode");
    m_pauseNode = createInputNode("pauseNode");
    m_toggleNode = createInputNode("toggleNode");
    connect(m_playNode, SIGNAL(dataChanged()), this, SLOT(onPlayNodeValueChanged()));
    connect(m_pauseNode, SIGNAL(dataChanged()), this, SLOT(onPauseNodeValueChanged()));
    connect(m_toggleNode, SIGNAL(dataChanged()), this, SLOT(onToggleNodeValueChanged()));

    m_activeNode = createOutputNode("activeNode");
    m_endNode = createOutputNode("endNode");
    m_positionNode = createOutputNode("positionNode");

    // setup Timer to be able to send a short pulse when end of file is reached:
    m_endPulseTimer.setSingleShot(true);
    m_endPulseTimer.setInterval(100);
    connect(&m_endPulseTimer, SIGNAL(timeout()), this, SLOT(onEndPulseTimeout()));

    connect(&m_player, SIGNAL(endOfFile()), this, SLOT(onEndOfFile()));
    connect(&m_player, SIGNAL(isPlayingChanged()), this, SIGNAL(isPlayingChanged()));
    connect(&m_player, SIGNAL(isPlayingChanged()), this, SLOT(onIsPlayingChanged()));
    connect(&m_player, SIGNAL(positionChanged()), this, SIGNAL(playbackPositionChanged()));
    connect(&m_player, SIGNAL(positionChanged()), this, SLOT(onPlaybackPositionChanged()));
    connect(&m_player, SIGNAL(lengthChanged()), this, SIGNAL(lengthChanged()));

    connect(&m_waveform, SIGNAL(pointsChanged()), this, SIGNAL(waveformChanged()));
    connect(&m_waveform, SIGNAL(availableChanged()), this, SIGNAL(waveformChanged()));
}

void AudioPlaybackBlock::getAdditionalState(QJsonObject& state) const {
    state["filePath"] = getFilePath();
    state["alwaysStartAtBegin"] = getAlwaysStartAtBegin();
    state["loop"] = getLoop();
    state["toggleMode"] = getToggleMode();
}

void AudioPlaybackBlock::setAdditionalState(const QJsonObject &state) {
    setFilePath(state["filePath"].toString());
    setAlwaysStartAtBegin(state["alwaysStartAtBegin"].toBool());
    setLoop(state["loop"].toBool());
    setToggleMode(state["toggleMode"].toBool());
}

void AudioPlaybackBlock::onPlayNodeValueChanged() {
    double value = m_playNode->getValue();
    if (value >= LuminosusConstants::triggerThreshold && m_lastPlayNodeValue < LuminosusConstants::triggerThreshold) {
        play();
    }
    m_lastPlayNodeValue = value;
}

void AudioPlaybackBlock::onPauseNodeValueChanged() {
    double value = m_pauseNode->getValue();
    if (value >= LuminosusConstants::triggerThreshold && m_lastPauseNodeValue < LuminosusConstants::triggerThreshold) {
        pause();
    }
    m_lastPauseNodeValue = value;
}

void AudioPlaybackBlock::onToggleNodeValueChanged() {
    double value = m_toggleNode->getValue();
    if (value >= LuminosusConstants::triggerThreshold && m_lastToggleNodeValue < LuminosusConstants::triggerThreshold) {
        onToggleDown();
    } else if (value < LuminosusConstants::triggerThreshold && m_lastToggleNodeValue >= LuminosusConstants::triggerThreshold) {
        onToggleUp();
    }
    m_lastToggleNodeValue = value;
}

void AudioPlaybackBlock::onEndOfFile() {
    // send "end pulse" with "End" Output Node:
    m_endNode->setValue(1.0);
    m_endPulseTimer.start();
    if (m_loop) {
        m_player.resetPosition();
        m_player.play();
    }
}

void AudioPlaybackBlock::onToggleDown() {
    if (m_toggleMode) {
        togglePlayPause();
    } else {
        play();
    }
}

void AudioPlaybackBlock::onToggleUp() {
    if (!m_toggleMode) {
        pause();
    }
}

void AudioPlaybackBlock::play() {
    if (m_alwaysStartAtBegin) {
        m_player.resetPosition();
    }
    if (!getIsPlaying()) {
        m_player.play();
    }
}

void AudioPlaybackBlock::pause() {
    if (getIsPlaying()) {
        m_player.pause();
    }
}

void AudioPlaybackBlock::togglePlayPause() {
    if (getIsPlaying()) {
        pause();
    } else {
        play();
    }
}

void AudioPlaybackBlock::onRemove() {
    pause();
}

void AudioPlaybackBlock::setFilePath(QString value) {
    if (value.isEmpty()) return;
    if (value == m_filePath) return;
    m_filePath = value;
    emit filePathChanged();
    m_player.loadFile(m_filePath);
    m_waveform.analyze(m_filePath);
}

QString AudioPlaybackBlock::getFileBaseName() const {
    return QFileInfo(m_filePath).baseName();
}

QString AudioPlaybackBlock::getFileFolder() const {
    int pathEnd = m_filePath.lastIndexOf("/");
    QString folder = m_filePath.left(pathEnd);
    qDebug() << folder;
    return folder;
}

QString AudioPlaybackBlock::getLengthString() const {
    const double length = m_player.getLength();
    return timeToString(length);
}

QString AudioPlaybackBlock::getPositionString() {
    const double position = m_player.getPosition();
    const double length = m_player.getLength();
    const double positionSec = length * position;
    return timeToString(positionSec);
}

QString AudioPlaybackBlock::timeToString(double seconds) {
    const int tenth = int(fmod(seconds, 1) * 10);
    const int sec = int(fmod(seconds, 60));
    const int min = int(seconds / 60);
    QString resultString = QString("%1'%2\"%3").arg(min, 2, 10, QChar('0'));
    resultString = resultString.arg(sec, 2, 10, QChar('0'));
    resultString = resultString.arg(tenth, 1, 10, QChar('0'));
    return resultString;
}

void AudioPlaybackBlock::onIsPlayingChanged() {
    if (getIsPlaying()) {
        m_activeNode->setValue(1.0);
    } else {
        m_activeNode->setValue(0.0);
    }
}

void AudioPlaybackBlock::onEndPulseTimeout() {
    m_endNode->setValue(0.0);
}

void AudioPlaybackBlock::onPlaybackPositionChanged() {
    m_positionNode->setValue(getPlaybackPosition());
}
