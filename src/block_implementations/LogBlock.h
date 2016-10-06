#ifndef LOGBLOCK_H
#define LOGBLOCK_H


#include "block_data/BlockBase.h"


class LogBlock : public BlockBase
{
	Q_OBJECT

public:

	static BlockInfo info() {
		static BlockInfo info;
		info.typeName = "App Log";
        info.nameInUi = "Application Log";
        info.category << "Debugging";
        info.dependencies = {BlockDependency::Debugging};
        info.helpText = "Shows the application log.";
		info.qmlFile = "qrc:/qml/Blocks/LogBlock.qml";
		info.complete<LogBlock>();
		return info;
	}

    explicit LogBlock(MainController* controller, QString uid);

    virtual QJsonObject getState() const override;
    virtual void setState(const QJsonObject& state) override;

public slots:
	virtual BlockInfo getBlockInfo() const override { return info(); }
};

#endif // LOGBLOCK_H
