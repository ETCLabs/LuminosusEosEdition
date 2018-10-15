#ifndef EOSMASTERPLAYBACKBLOCK_H
#define EOSMASTERPLAYBACKBLOCK_H

#include "core/block_data/BlockBase.h"
#include "utils.h"


class EosMasterPlaybackBlock: public BlockBase
{
    Q_OBJECT

public:

    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Eos Master Playback";
        info.nameInUi = "Master Playback";
        info.category << "ETC Consoles" << "Eos";
        info.helpText = "Controls the Master Playback of the connected console.\n\n"
                        "Attention: The shown active and pending Cues may not be the ones "
                        "that are loaded in the Master Playback but could also come from a "
                        "different Playback like a Fader.\n"
                        "(This is a limitation of the OSC implementation.)\n\n"
                        "This block uses Eos OSC user 1.";
        info.qmlFile = "qrc:/qml/Blocks/Eos/EosMasterPlaybackBlock.qml";
        info.complete<EosMasterPlaybackBlock>();
        return info;
    }

    explicit EosMasterPlaybackBlock(MainController* controller, QString uid);

    // block needs to be rendered for MIDI mapping to work
    virtual bool renderIfNotVisible() const override { return true; }

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    void sendLoadEvent(qreal buttonEdge);
    void sendUnloadEvent();
    void sendStopEvent(qreal buttonEdge);
    void sendGoEvent(qreal buttonEdge);

};

#endif // EOSMASTERPLAYBACKBLOCK_H
