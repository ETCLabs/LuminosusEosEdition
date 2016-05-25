#ifndef RANDOMVALUEBLOCK_H
#define RANDOMVALUEBLOCK_H

#include "block_data/OneOutputBlock.h"
#include "utils.h"

#include <QtMath>


class RandomValueBlock : public OneOutputBlock
{
	Q_OBJECT

	Q_PROPERTY(double fadeRatio READ getFadeRatio WRITE setFadeRatio NOTIFY fadeRatioChanged)
	Q_PROPERTY(double stepTime READ getStepTime WRITE setStepTime NOTIFY stepTimeChanged)

public:

	static BlockInfo info() {
		static BlockInfo info;
		info.name = "Random Value";
		info.category = QStringList {"Generator"};
		info.helpText = "Outputs a random value.\n"
						"'Step' is the time between two values.\n"
						"'Fade' is the part of the step time that is used to fade to the next value.";
		info.qmlFile = "qrc:/qml/Blocks/RandomValueBlock.qml";
		info.complete<RandomValueBlock>();
		return info;
	}

	explicit RandomValueBlock(MainController* controller, QString uid);

signals:
	void fadeRatioChanged();
	void stepTimeChanged();

public slots:
	virtual BlockInfo getBlockInfo() const override { return info(); }

	double getFadeRatio() const { return m_fadeRatio; }
	void setFadeRatio(double value) { m_fadeRatio = limit(0, value, 1); emit fadeRatioChanged(); }

	double getStepTime() const { return m_stepTime; }
	void setStepTime(double value) { m_stepTime = qMax(0.0, value); emit stepTimeChanged(); }

private:
	static double getRandomValue() {
		return static_cast<float>(qrand()) / static_cast<float>(RAND_MAX);
	}

private slots:
	void eachFrame(double timeSinceLastFrame);

protected:
	double m_fadeRatio;
	double m_stepTime;

	double m_oldValue;
	double m_newValue;
	double m_pos;

};

#endif // RANDOMVALUEBLOCK_H
