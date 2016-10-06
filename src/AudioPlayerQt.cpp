#include "AudioPlayerQt.h"

#include "utils.h"
#include <QDebug>


AudioPlayerQt::AudioPlayerQt(QObject *parent)
    : QObject(parent)
    , m_volume(1.0)
    , m_length(0.0)
    , m_position(0.0)
{
    m_player.setNotifyInterval(100 /*ms*/);
    connect(&m_player, SIGNAL(stateChanged(QMediaPlayer::State)), this, SIGNAL(isPlayingChanged()));
    connect(&m_player, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)), this, SLOT(onMediaStatusChanged()));
    connect(&m_player, SIGNAL(positionChanged(qint64)), this, SLOT(onPositionChanged()));
    connect(&m_player, SIGNAL(durationChanged(qint64)), this, SLOT(onDurationChanged()));
}

void AudioPlayerQt::loadFile(QString filePath) {
    // reset length:
    m_length = 0.0;
    m_player.setMedia(QUrl(filePath));
    m_player.duration();
    const qint64 lengthInMs = m_player.duration();
    if (lengthInMs > 0) {
        m_length = lengthInMs / 1000.0;
    }
}

bool AudioPlayerQt::getIsPlaying() const {
    return m_player.state() == QMediaPlayer::PlayingState;
}

void AudioPlayerQt::play() {
    m_player.play();
}

void AudioPlayerQt::stop() {
    m_player.stop();
}

void AudioPlayerQt::pause() {
    m_player.pause();
}

void AudioPlayerQt::resetPosition() {
    m_player.setPosition(0.0);
}

void AudioPlayerQt::setVolume(double value) {
    m_volume = limit(0.0, value, 1.0);
    m_player.setVolume(m_volume * 100);
}

void AudioPlayerQt::setPosition(double value) {
    const qint64 posInMs = limit(0, value, 1) * m_length * 1000;
    m_player.setPosition(posInMs);
}

void AudioPlayerQt::onMediaStatusChanged() {
    if (m_player.mediaStatus() == QMediaPlayer::EndOfMedia) {
        emit endOfFile();
    }
}

void AudioPlayerQt::onPositionChanged() {
    const double duration = double(m_player.duration());
    const qint64 pos = m_player.position();
    if (duration > 0) {
        m_position = limit(0, pos / duration, 1);
    } else {
        m_position = 0;
    }
    emit positionChanged();
}

void AudioPlayerQt::onDurationChanged() {
    const qint64 duration = m_player.duration();
    if (duration > 0) {
        m_length = duration / 1000.0;
    } else {
        m_length = 0;
    }
    emit lengthChanged();
    emit positionChanged();
}
