#ifndef AUDIOPLAYBACKBLOCK_H
#define AUDIOPLAYBACKBLOCK_H

#include "core/block_data/BlockBase.h"
#include "audio/AudioWaveform.h"
#include "utils.h"

#ifdef USE_VLC_LIB
#include "audio/AudioPlayerVlc.h"
#else
#include "audio/AudioPlayerQt.h"
#endif

#include <QTimer>


class AudioPlaybackBlock : public BlockBase
{
    Q_OBJECT

    Q_PROPERTY(QString filePath READ getFilePath WRITE setFilePath NOTIFY filePathChanged)
    Q_PROPERTY(QString fileBaseName READ getFileBaseName NOTIFY filePathChanged)
    Q_PROPERTY(bool alwaysStartAtBegin READ getAlwaysStartAtBegin WRITE setAlwaysStartAtBegin NOTIFY alwaysStartAtBeginChanged)
    Q_PROPERTY(bool loop READ getLoop WRITE setLoop NOTIFY loopChanged)
    Q_PROPERTY(bool toggleMode READ getToggleMode WRITE setToggleMode NOTIFY toggleModeChanged)

    Q_PROPERTY(bool isPlaying READ getIsPlaying NOTIFY isPlayingChanged)
    Q_PROPERTY(QString lengthString READ getLengthString NOTIFY lengthChanged)
    Q_PROPERTY(QString positionString READ getPositionString NOTIFY playbackPositionChanged)
    Q_PROPERTY(double playbackPosition READ getPlaybackPosition WRITE setPlaybackPosition NOTIFY playbackPositionChanged)
    Q_PROPERTY(double length READ getLength NOTIFY lengthChanged)

    Q_PROPERTY(const QVector<double>& waveform READ getWaveform NOTIFY waveformChanged)
    Q_PROPERTY(bool waveformIsAvailable READ waveformIsAvailable NOTIFY waveformChanged)


public:

    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Audio Playback";
        info.nameInUi = "Audio Playback";
        info.category << "General" << "Media + Presentation";
        info.availabilityRequirements << AvailabilityRequirement::LocalFileSelection;
        info.helpText = "Plays an audio file.\n\n"
                        "If the 'Toggle' option is enabled, the first press on the play button "
                        "starts playback and the second one pauses it.\n"
                        "If 'Toggle' is disabled, audio is only played while the button is pressed.\n"
                        "The 'Both' input is also affected by this setting.\n\n"
                        "If the 'From Begin' option is enabled, playback always starts from begin.\n"
                        "If the 'Loop' option is enabled, the playback will loop indefinitely.\n\n"
                        "The waveform is only displayed for .wav files.";
        info.qmlFile = "qrc:/qml/Blocks/Audio/AudioPlaybackBlock.qml";
        info.complete<AudioPlaybackBlock>();
        return info;
    }

    explicit AudioPlaybackBlock(MainController* controller, QString uid);

    virtual void getAdditionalState(QJsonObject& state) const override;
    virtual void setAdditionalState(const QJsonObject& state) override;

signals:
    void filePathChanged();
    void alwaysStartAtBeginChanged();
    void loopChanged();
    void toggleModeChanged();

    void isPlayingChanged();
    void playbackPositionChanged();
    void lengthChanged();

    void waveformChanged();

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    void onPlayNodeValueChanged();
    void onPauseNodeValueChanged();
    void onToggleNodeValueChanged();

    void onEndOfFile();

    // -------------- Logic ------------------

    void onToggleDown();
    void onToggleUp();

    void play();
    void pause();
    void togglePlayPause();

    virtual void onRemove() override;

    // -------------- Getter + Setter for properties --------------------

    QString getFilePath() const { return m_filePath; }
    void setFilePath(QString value);

    QString getFileBaseName() const;
    QString getFileFolder() const;

    bool getIsPlaying() const { return m_player.getIsPlaying(); }

    QString getLengthString() const;
    QString getPositionString();

    double getPlaybackPosition() const { return m_player.getPosition(); }
    void setPlaybackPosition(double value) { return m_player.setPosition(value); }

    double getLength() const { return m_player.getLength(); }

    bool getAlwaysStartAtBegin() const { return m_alwaysStartAtBegin; }
    void setAlwaysStartAtBegin(bool value) { m_alwaysStartAtBegin = value; emit alwaysStartAtBeginChanged(); }

    bool getLoop() const { return m_loop; }
    void setLoop(bool value) { m_loop = value; emit loopChanged(); }

    bool getToggleMode() const { return m_toggleMode; }
    void setToggleMode(bool value) { m_toggleMode = value; emit toggleModeChanged(); }

    const QVector<double>& getWaveform() const { return m_waveform.getPoints(); }
    bool waveformIsAvailable() const { return m_waveform.isAvailable(); }


    // -------------- other Getter + Setter --------------------

    double getVolume() const { return m_player.getVolume(); }
    void setVolume(double value) { m_player.setVolume(value); }

    virtual void onControllerPressed() override { onToggleDown(); }
    virtual void onControllerReleased(double) override { onToggleUp(); }
    virtual void onControllerRotated(double relativeAmount, double, bool) override { setPlaybackPosition(limitToOne(getPlaybackPosition() + relativeAmount)); }

private slots:
    static QString timeToString(double seconds);

    void onIsPlayingChanged();

    void onEndPulseTimeout();

    void onPlaybackPositionChanged();

protected:
    QPointer<NodeBase> m_playNode;
    QPointer<NodeBase> m_pauseNode;
    QPointer<NodeBase> m_toggleNode;

    QPointer<NodeBase> m_activeNode;
    QPointer<NodeBase> m_endNode;
    QPointer<NodeBase> m_positionNode;

    QString m_filePath;
    double m_lastPlayNodeValue;
    double m_lastPauseNodeValue;
    double m_lastToggleNodeValue;

    bool m_alwaysStartAtBegin;
    bool m_loop;
    bool m_toggleMode;

    QTimer m_endPulseTimer;

#ifdef USE_VLC_LIB
    AudioPlayerVlc m_player;
#else
    AudioPlayerQt m_player;
#endif

    AudioWaveform m_waveform;
};

#endif // AUDIOPLAYBACKBLOCK_H
