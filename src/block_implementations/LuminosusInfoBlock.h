#ifndef LUMINOSUSINFOBLOCK_H
#define LUMINOSUSINFOBLOCK_H

#include "block_data/BlockBase.h"


class LuminosusInfoBlock: public BlockBase
{
    Q_OBJECT

public:

    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Luminosus Info";
        info.nameInUi = "Version Information";
        info.category << "Debugging";
        info.dependencies = {BlockDependency::Debugging};
        info.helpText = "Shows some useful information about the version of this software.";
        info.qmlFile = "qrc:/qml/Blocks/LuminosusInfoBlock.qml";
        info.complete<LuminosusInfoBlock>();
        return info;
    }

    explicit LuminosusInfoBlock(MainController* controller, QString uid);

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    QString getQtCompileVersion() const { return QT_VERSION_STR; }
    QString getQtRuntimeVersion() const { return qVersion(); }
};

#endif // LUMINOSUSINFOBLOCK_H
