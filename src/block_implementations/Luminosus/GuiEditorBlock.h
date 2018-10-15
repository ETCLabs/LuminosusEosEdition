#ifndef GUIEDITORBLOCK_H
#define GUIEDITORBLOCK_H

#include "core/block_data/BlockBase.h"
#include "core/SmartAttribute.h"


class GuiEditorBlock: public BlockBase
{
    Q_OBJECT

public:

    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "GUI Editor";
        info.nameInUi = "GUI Editor";
        info.category << "General" << "Debugging";
        info.visibilityRequirements << VisibilityRequirement::DeveloperMode;
        info.helpText = "Provides live editing of the QML part of the currently selected block.";
        info.qmlFile = "qrc:/qml/Blocks/Luminosus/GuiEditorBlock.qml";
        info.complete<GuiEditorBlock>();
        return info;
    }

    explicit GuiEditorBlock(MainController* controller, QString uid);

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    void apply();

private slots:
    void onFocusChanged();
    void onCodeChanged();

protected:
    StringAttribute m_code;
    BoolAttribute m_saved;

};

#endif // GUIEDITORBLOCK_H
