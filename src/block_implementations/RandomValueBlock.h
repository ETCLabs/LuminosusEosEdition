#ifndef RANDOMVALUEBLOCK_H
#define RANDOMVALUEBLOCK_H

#include "block_data/InOutBlock.h"
#include "utils.h"

#include <QtMath>


class RandomValueBlock : public InOutBlock
{
	Q_OBJECT

	Q_PROPERTY(double fadeRatio READ getFadeRatio WRITE setFadeRatio NOTIFY fadeRatioChanged)
	Q_PROPERTY(double stepTime READ getStepTime WRITE setStepTime NOTIFY stepTimeChanged)
    Q_PROPERTY(double timeVariation READ getTimeVariation WRITE setTimeVariation NOTIFY timeVariationChanged)

public:

	static BlockInfo info() {
		static BlockInfo info;
		info.typeName = "Random Value";
        info.category << "Generator";
		info.helpText = "Outputs a random value.\n"
                        "'Step' is the time between two values.\n"
                        "'Step +/-' is the variation of the time between two values.\n"
						"'Fade' is the part of the step time that is used to fade to the next value.";
		info.qmlFile = "qrc:/qml/Blocks/RandomValueBlock.qml";
		info.complete<RandomValueBlock>();
		return info;
	}

	explicit RandomValueBlock(MainController* controller, QString uid);

    QJsonObject getState() const override;
    void setState(const QJsonObject& state) override;

signals:
	void fadeRatioChanged();
	void stepTimeChanged();
    void timeVariationChanged();

public slots:
	virtual BlockInfo getBlockInfo() const override { return info(); }

    void onInputChanged();

    void sync();

	double getFadeRatio() const { return m_fadeRatio; }
	void setFadeRatio(double value) { m_fadeRatio = limit(0, value, 1); emit fadeRatioChanged(); }

	double getStepTime() const { return m_stepTime; }
    void setStepTime(double value) { m_stepTime = qMax(0.001, value); emit stepTimeChanged(); }

    double getTimeVariation() const { return m_timeVaration; }
    void setTimeVariation(double value) { m_timeVaration = limit(0, value, 1); emit timeVariationChanged(); }

private:
    static double getRandomValue();

private slots:
	void eachFrame(double timeSinceLastFrame);

protected:
	double m_fadeRatio;
	double m_stepTime;
    double m_timeVaration;

	double m_oldValue;
	double m_newValue;
    double m_currentStepTime;
	double m_pos;

};

#endif // RANDOMVALUEBLOCK_H
