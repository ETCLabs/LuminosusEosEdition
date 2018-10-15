#ifndef COMMANDLINEBLOCK_H
#define COMMANDLINEBLOCK_H

#include "core/block_data/OneInputBlock.h"
#include "core/SmartAttribute.h"


class CommandlineBlock : public OneInputBlock
{
    Q_OBJECT

public:

    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Commandline";
        info.nameInUi = "Commandline";
        info.category << "General" << "Logic";
        info.helpText = "Executes a bash command. Only works on Linux at the moment.";
        info.visibilityRequirements << VisibilityRequirement::DeveloperMode;
        info.qmlFile = "qrc:/qml/Blocks/Logic/CommandlineBlock.qml";
        info.complete<CommandlineBlock>();
        return info;
    }

    explicit CommandlineBlock(MainController* controller, QString uid);

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    void execute();

protected:
    StringAttribute m_command;
};

#endif // COMMANDLINEBLOCK_H
