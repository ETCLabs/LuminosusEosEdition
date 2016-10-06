#ifndef SLIDERBLOCK_H
#define SLIDERBLOCK_H

#include "block_data/OneOutputBlock.h"


class SliderBlock : public OneOutputBlock
{
    Q_OBJECT

public:

	static BlockInfo info() {
		static BlockInfo info;
		info.typeName = "Slider";
        info.category << "Controls";
        info.helpText = "A simple slider that outputs values between 0 and 1.";
		info.qmlFile = "qrc:/qml/Blocks/SliderBlock.qml";
		info.complete<SliderBlock>();
		return info;
	}

	explicit SliderBlock(MainController* controller, QString uid) : OneOutputBlock(controller, uid, info().qmlFile) {

	}

	virtual QJsonObject getState() const override;
	virtual void setState(const QJsonObject& state) override;

public slots:
	virtual BlockInfo getBlockInfo() const override { return info(); }

    virtual void onControllerPressed() override;
    virtual void onControllerReleased(double) override;
    virtual void onControllerRotated(double value, bool) override;

protected:
	double m_valueBeforeFlash = 0.0;
};

#endif // SLIDERBLOCK_H
