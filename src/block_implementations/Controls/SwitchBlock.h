#ifndef SWITCHBLOCK
#define SWITCHBLOCK

#include "core/block_data/OneOutputBlock.h"
#include "core/SmartAttribute.h"


class SwitchBlock : public OneOutputBlock
{
    Q_OBJECT

public:

	static BlockInfo info() {
		static BlockInfo info;
		info.typeName = "Switch";
        info.category << "General" << "Controls";
        info.helpText = "A simple switch that outputs either 0 or 1.";
        info.qmlFile = "qrc:/qml/Blocks/Controls/SwitchBlock.qml";
		info.complete<SwitchBlock>();
		return info;
	}

    explicit SwitchBlock(MainController* controller, QString uid);

public slots:
	virtual BlockInfo getBlockInfo() const override { return info(); }
    virtual void onControllerPressed() override;
};

#endif // SWITCHBLOCK

