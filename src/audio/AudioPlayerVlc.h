#ifndef AUDIOPLAYERVLC_H
#define AUDIOPLAYERVLC_H

#include <QObject>

#include <VLCQtCore/Common.h>
#include <VLCQtCore/Instance.h>
#include <VLCQtCore/Media.h>
#include <VLCQtCore/MediaPlayer.h>
#include <VLCQtCore/Audio.h>


/**
 * @brief The AudioPlayerVlc class can play audio files. It uses VlcMediaPlayer.
 */
class AudioPlayerVlc : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief AudioPlayerVlc creates an AudioPlayerVlc object
     * @param parent QObject parent
     */
    explicit AudioPlayerVlc(QObject* parent = 0);

signals:
    void isPlayingChanged();
    void positionChanged();
    void lengthChanged();
    void endOfFile();

public slots:

    // ------------- Logic -----------------

    /**
     * @brief loadFile loads the specified file without playing it
     * @param filePath path to an audio file
     */
    void loadFile(QString filePath);

    /**
     * @brief play starts playback of loaded audio
     */
    void play();

    /**
     * @brief stop stops playback and resets playback position
     */
    void stop();

    /**
     * @brief pause pauses playback without changing playback position
     */
    void pause();

    /**
     * @brief resetPosition resets the playback position to the begin
     */
    void resetPosition();

    // ------------- Getter + Setter -----------------

    /**
     * @brief getIsPlaying returns true if audio is playing
     * @return true if audio is playing
     */
    bool getIsPlaying() const { return m_isPlaying; }

    /**
     * @brief getVolume returns the current volume of this player
     * @return volume level between 0 and 1
     */
    double getVolume() const { return m_volume; }
    /**
     * @brief setVolume sets the volume of this player
     * @param value volume level between 0 and 1
     */
    void setVolume(double value);

    /**
     * @brief getPosition returns the relative playback position
     * @return a position between 0 and 1
     */
    double getPosition() const { return m_position; }

    /**
     * @brief setPosition sets the relative playback position
     * @param value a position between 0 and 1
     */
    void setPosition(double value);

    /**
     * @brief getLength returns the length of the loaded file in seconds
     * @return length in seconds or 0 if nothing is loaded
     */
    double getLength() const { return m_length; }


private slots:

    /**
     * @brief updateMetadata updates the length field when media is parsed
     */
    void updateMetadata();

    /**
     * @brief onStateChanged updates isPlaying state
     */
    void onStateChanged();

    /**
     * @brief onPositionChanged updates the position value
     */
    void onPositionChanged();

    /**
     * @brief onEndOfFile reloads the media file to be able to play it again
     */
    void onEndOfFile();


protected:
    /**
     * @brief m_vlcInstance VLC instace used for m_player
     */
    VlcInstance m_vlcInstance;
    /**
     * @brief m_vlcMedia currently used VLC media object
     */
    VlcMedia* m_vlcMedia;
    /**
     * @brief m_player is the VlcMediaPlayer object to playback audio files
     */
    VlcMediaPlayer m_player;

    /**
     * @brief m_volume current volume between 0 and 1 (not used anmyore?)
     */
    double m_volume;

    /**
     * @brief m_length the length of the loaded file in seconds
     */
    double m_length;

    /**
     * @brief m_position the relative position between 0 and 1
     */
    double m_position;

    /**
     * @brief m_isPlaying is true if audio is playing
     */
    bool m_isPlaying;
};


#endif // AUDIOPLAYERVLC_H
