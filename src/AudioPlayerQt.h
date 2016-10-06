#ifndef AUDIOPLAYERQT_H
#define AUDIOPLAYERQT_H

#include <QObject>
#include <QMediaPlayer>


/**
 * @brief The AudioPlayerQt class can play audio files. It uses QMediaPlayer.
 */
class AudioPlayerQt : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief AudioPlayerQt creates an AudioPlayerQt object
     * @param parent QObject parent
     */
    explicit AudioPlayerQt(QObject* parent = 0);

signals:
    void isPlayingChanged();
    void positionChanged();
    void lengthChanged();
    void endOfFile();

public slots:
    /**
     * @brief loadFile loads the specified file without playing it
     * @param filePath path to an audio file
     */
    void loadFile(QString filePath);

    /**
     * @brief isPlaying returns true if audio is playing
     * @return true if audio is playing
     */
    bool getIsPlaying() const;

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
     * @brief onMediaStatusChanged checks if the player has reached the end of file
     */
    void onMediaStatusChanged();

    /**
     * @brief onPositionChanged updates the position value
     */
    void onPositionChanged();

    /**
     * @brief onDurationChanged updates the length value
     */
    void onDurationChanged();

protected:
    /**
     * @brief m_player the QMediaPlayer object to play the audio files
     */
    QMediaPlayer m_player;

    /**
     * @brief m_volume current volume between 0 and 1
     */
    double m_volume;

    /**
     * @brief m_length the length of the loaded file in seconds
     */
    double m_length;

    /**
     * @brief m_position is the relative position between 0 and 1
     */
    double m_position;
};

#endif // AUDIOPLAYERQT_H
