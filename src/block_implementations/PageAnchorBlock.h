#ifndef PAGEANCHORBLOCK_H
#define PAGEANCHORBLOCK_H

#include "block_data/BlockBase.h"
#include "utils.h"


class PageAnchorBlock: public BlockBase
{
    Q_OBJECT

    Q_PROPERTY(int anchorIndex READ getAnchorIndex NOTIFY anchorIndexChanged)

public:

    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Page Anchor";
        info.nameInUi = "Page Anchor";
        info.category << "Presentation";
        info.qmlFile = "qrc:/qml/Blocks/PageAnchorBlock.qml";
        info.complete<PageAnchorBlock>();
        return info;
    }

    explicit PageAnchorBlock(MainController* controller, QString uid);
    ~PageAnchorBlock();

    virtual QJsonObject getState() const override;

    virtual void setState(const QJsonObject& state) override;

    virtual bool renderIfNotVisible() const override { return true; }

signals:
    void anchorIndexChanged();

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    int getAnchorIndex();
};
#endif // PAGEANCHORBLOCK_H
