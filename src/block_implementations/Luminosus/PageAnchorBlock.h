#ifndef PAGEANCHORBLOCK_H
#define PAGEANCHORBLOCK_H

#include "core/block_data/BlockBase.h"


class PageAnchorBlock: public BlockBase
{
    Q_OBJECT

    Q_PROPERTY(int anchorIndex READ getAnchorIndex NOTIFY anchorIndexChanged)

public:

    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Page Anchor";
        info.nameInUi = "Page Anchor";
        info.category << "General" << "Presentation";
		info.visibilityRequirements << VisibilityRequirement::StandaloneVersion;
        info.qmlFile = "qrc:/qml/Blocks/Luminosus/PageAnchorBlock.qml";
        info.complete<PageAnchorBlock>();
        return info;
    }

    explicit PageAnchorBlock(MainController* controller, QString uid);
    ~PageAnchorBlock();

signals:
    void anchorIndexChanged();

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    int getAnchorIndex();
};

#endif // PAGEANCHORBLOCK_H
