#ifndef CUELISTHELPERBLOCK_H
#define CUELISTHELPERBLOCK_H

#include "core/block_data/BlockBase.h"


class CueListHelperBlock : public BlockBase
{
    Q_OBJECT

public:

    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Cue List Helper";
        info.nameInUi = "Cue List Helper";
        info.category << "Theater";
        info.visibilityRequirements << VisibilityRequirement::StandaloneVersion;
        info.helpText = "Contains additional Cue List helper functions.\n\n"
                        "'Pause & Reset Lists' pauses and resets all Cue Lists.\n\n"
                        "'Clear Benches' clears all Working Benches of Theater Blocks\n\n"
                        "This can be used to get to a neutral state before starting a Cue List.";
        info.qmlFile = "qrc:/qml/Blocks/Theater/CueListHelperBlock.qml";
        info.complete<CueListHelperBlock>();
        return info;
    }

    explicit CueListHelperBlock(MainController* controller, QString uid);

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    void clearLists();

    void clearBenches();
};

#endif // CUELISTHELPERBLOCK_H
