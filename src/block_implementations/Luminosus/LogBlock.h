#ifndef LOGBLOCK_H
#define LOGBLOCK_H


#include "core/block_data/BlockBase.h"


class LogBlock : public BlockBase
{
	Q_OBJECT

public:

	static BlockInfo info() {
		static BlockInfo info;
		info.typeName = "App Log";
        info.nameInUi = "Application Log";
        info.category << "General" << "Debugging";
        info.visibilityRequirements << VisibilityRequirement::DeveloperMode;
        info.helpText = "Shows the application log.";
        info.qmlFile = "qrc:/qml/Blocks/Luminosus/LogBlock.qml";
		info.complete<LogBlock>();
		return info;
	}

    explicit LogBlock(MainController* controller, QString uid);

public slots:
	virtual BlockInfo getBlockInfo() const override { return info(); }
};

#endif // LOGBLOCK_H
