#ifndef AUDIOSTREAMINGBLOCK_H
#define AUDIOSTREAMINGBLOCK_H

#include "core/Nodes.h"
#include "core/block_data/OneInputBlock.h"
#include "core/SmartAttribute.h"

#include <QMediaPlayer>


class AudioStreamingBlock : public OneInputBlock
{
    Q_OBJECT

public:

    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Audio Streaming";
        info.nameInUi = "Audio Streaming";
        info.category << "General" << "Media + Presentation";
        info.helpText = "Plays an audio stream from the internet, for example an internet radio station.\n\n"
                        "The URL must point to an audio file (i.e. ending with '.mp3')\n"
                        "Streams ending with '.m3u' must be opened in a text editor first and the "
                        "URL inside must be used.";
        info.qmlFile = "qrc:/qml/Blocks/Audio/AudioStreamingBlock.qml";
        info.complete<AudioStreamingBlock>();
        return info;
    }

    explicit AudioStreamingBlock(MainController* controller, QString uid);

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    void play();
    void stop();

protected:
    QPointer<NodeBase> m_stopNode;
    StringAttribute m_url;

    QMediaPlayer m_player;

};

#endif // AUDIOSTREAMINGBLOCK_H
