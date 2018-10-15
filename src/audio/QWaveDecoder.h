#ifndef QWAVEDECODER_H
#define QWAVEDECODER_H

#include <QIODevice>
#include <QAudioFormat>


class QWaveDecoder : public QIODevice
{
    Q_OBJECT

public:
    explicit QWaveDecoder(QIODevice *source, QObject *parent = 0);
    ~QWaveDecoder();

    QAudioFormat audioFormat() const;
    int duration() const;

    qint64 size() const override;
    bool isSequential() const override;
    qint64 bytesAvailable() const override;

Q_SIGNALS:
    void formatKnown();
    void parsingError();

private Q_SLOTS:
    void handleData();

private:
    qint64 readData(char *data, qint64 maxlen) override;
    qint64 writeData(const char *data, qint64 len) override;

    bool enoughDataAvailable();
    bool findChunk(const char *chunkId);
    void discardBytes(qint64 numBytes);
    void parsingFailed();

    enum State {
        InitialState,
        WaitingForFormatState,
        WaitingForDataState
    };

    struct chunk
    {
        char        id[4];
        quint32     size;
    };
    bool peekChunk(chunk* pChunk, bool handleEndianness = true);

    struct RIFFHeader
    {
        chunk       descriptor;
        char        type[4];
    };
    struct WAVEHeader
    {
        chunk       descriptor;
        quint16     audioFormat;
        quint16     numChannels;
        quint32     sampleRate;
        quint32     byteRate;
        quint16     blockAlign;
        quint16     bitsPerSample;
    };

    bool haveFormat;
    qint64 dataSize;
    QAudioFormat format;
    QIODevice *source;
    State state;
    quint32 junkToSkip;
    bool bigEndian;
};

#endif // QWAVEDECODER_H
