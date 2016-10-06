#include "AudioPlayerVlc.h"

#include "utils.h"
#include <QDebug>


AudioPlayerVlc::AudioPlayerVlc(QObject *parent)
    : QObject(parent)
    , m_vlcInstance(VlcCommon::args(), this)
    , m_vlcMedia(nullptr)
    , m_player(&m_vlcInstance)
    , m_volume(1.0)
    , m_length(0.0)
    , m_position(0.0)
    , m_isPlaying(false)
{
    connect(&m_player, SIGNAL(end()), this, SLOT(onEndOfFile()));
    connect(&m_player, SIGNAL(stateChanged()), this, SLOT(onStateChanged()));
    connect(&m_player, SIGNAL(positionChanged(float)), this, SLOT(onPositionChanged()));
}

void AudioPlayerVlc::loadFile(QString filePath) {
    filePath = filePath.remove("file://");
    if (filePath.isEmpty()) return;

    VlcMedia* newMedia = new VlcMedia(filePath, true, &m_vlcInstance);
    if (!newMedia) return;

    connect(newMedia, SIGNAL(parsedChanged(bool)), this, SLOT(updateMetadata()));
    newMedia->parse();

    m_player.openOnly(newMedia);

    if (m_player.audio()) {
        m_player.audio()->setVolume(100);
    }
    if (m_vlcMedia) {
        delete m_vlcMedia;
    }
    m_vlcMedia = newMedia;
}

void AudioPlayerVlc::play() {
    m_player.play();
    if ((1 - m_position) * m_length > 0.2 /*sec*/) {
        m_player.setPosition(m_position);
    }
}

void AudioPlayerVlc::stop() {
    m_player.stop();
}

void AudioPlayerVlc::pause() {
    m_player.pause();
}

void AudioPlayerVlc::resetPosition() {
    m_player.setPosition(0.0);
    m_position = 0.0;
    emit positionChanged();
}

void AudioPlayerVlc::setVolume(double value) {
    m_volume = limit(0.0, value, 1.0);
    if (m_player.audio()) {
        m_player.audio()->setVolume(m_volume * 100);
    }
}

void AudioPlayerVlc::setPosition(double value) {
    m_player.setPosition(limit(0, value, 1));
    m_position = value;
    emit positionChanged();
}

void AudioPlayerVlc::updateMetadata() {
    const int lengthInMs = m_vlcMedia->duration();
    if (lengthInMs > 0) {
        m_length = lengthInMs / 1000.0;
    }
    emit lengthChanged();
}

void AudioPlayerVlc::onStateChanged() {
    if (m_player.state() == Vlc::Playing) {
        m_isPlaying = true;
    } else {
        m_isPlaying = false;
    }
    emit isPlayingChanged();
}

void AudioPlayerVlc::onPositionChanged() {
    m_position = m_player.position();
    emit positionChanged();
}

void AudioPlayerVlc::onEndOfFile() {
    m_player.stop();
    m_player.openOnly(m_vlcMedia);
    if (m_player.audio()) {
        m_player.audio()->setVolume(100);
    }
    emit endOfFile();
}
