#ifndef VIDEOBLOCK_H
#define VIDEOBLOCK_H

#include "core/block_data/BlockBase.h"


class VideoBlock : public BlockBase
{
    Q_OBJECT

public:

    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Video";
        info.nameInUi = "Video";
        info.category << "General" << "Media + Presentation";
        info.availabilityRequirements << AvailabilityRequirement::LocalFileSelection;
        info.visibilityRequirements << VisibilityRequirement::DeveloperMode;
        info.helpText = "Displays a video file.";
        info.qmlFile = "qrc:/qml/Blocks/Info/VideoBlock.qml";
        info.complete<VideoBlock>();
        return info;
    }

    explicit VideoBlock(MainController* controller, QString uid);

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

protected:
    StringAttribute m_filePath;
};

#endif // VIDEOBLOCK_H
