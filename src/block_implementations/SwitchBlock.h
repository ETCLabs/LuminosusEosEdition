#ifndef SWITCHBLOCK
#define SWITCHBLOCK

#include "block_data/OneOutputBlock.h"

class SwitchBlock : public OneOutputBlock
{
    Q_OBJECT

public:

	static BlockInfo info() {
		static BlockInfo info;
		info.typeName = "Switch";
        info.category << "Controls";
        info.helpText = "A simple switch that outputs either 0 or 1.";
		info.qmlFile = "qrc:/qml/Blocks/SwitchBlock.qml";
		info.complete<SwitchBlock>();
		return info;
	}

	explicit SwitchBlock(MainController* controller, QString uid);

	// getState is inherited from OneOutputBlock

	virtual void setState(const QJsonObject& state) override;

public slots:
	virtual BlockInfo getBlockInfo() const override { return info(); }
    virtual void onControllerPressed() override;
};

#endif // SWITCHBLOCK

