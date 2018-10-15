#ifndef SLIDERBLOCK_H
#define SLIDERBLOCK_H

#include "core/block_data/OneOutputBlock.h"


class SliderBlock : public OneOutputBlock
{
    Q_OBJECT

public:

	static BlockInfo info() {
		static BlockInfo info;
		info.typeName = "Slider";
        info.category << "General" << "Controls";
        info.helpText = "A simple slider that outputs a value between 0 and 1.";
        info.qmlFile = "qrc:/qml/Blocks/Controls/SliderBlock.qml";
		info.complete<SliderBlock>();
		return info;
	}

    explicit SliderBlock(MainController* controller, QString uid);

	virtual void getAdditionalState(QJsonObject& state) const override;
	virtual void setAdditionalState(const QJsonObject& state) override;

public slots:
	virtual BlockInfo getBlockInfo() const override { return info(); }

    virtual void onControllerPressed() override;
    virtual void onControllerReleased(double) override;
    virtual void onControllerRotated(double relativeAmount, double, bool) override;

protected:
	double m_valueBeforeFlash = 0.0;
};

#endif // SLIDERBLOCK_H
