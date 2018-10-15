#ifndef RANDOMVALUEBLOCK_H
#define RANDOMVALUEBLOCK_H

#include "core/block_data/InOutBlock.h"
#include "core/SmartAttribute.h"
#include "core/Matrix.h"
#include "utils.h"

#include <QtMath>


class RandomValueBlock : public InOutBlock
{
    Q_OBJECT

public:

	static BlockInfo info() {
		static BlockInfo info;
		info.typeName = "Random Value";
        info.category << "General" << "Generator";
        info.helpText = "Generates a random values.\n"
                        "'Step' is the time between two values.\n"
                        "'Step +/-' is the variation of the time between two values.\n"
						"'Fade' is the part of the step time that is used to fade to the next value.";
        info.qmlFile = "qrc:/qml/Blocks/Logic/RandomValueBlock.qml";
		info.complete<RandomValueBlock>();
		return info;
	}

    explicit RandomValueBlock(MainController* controller, QString uid);

public slots:
	virtual BlockInfo getBlockInfo() const override { return info(); }

    void onInputChanged();

    void sync();

private:
    void fillRandom(HsvMatrix& matrix) const;

private slots:
	void eachFrame(double timeSinceLastFrame);

    void updateRequestedSize();

protected:
    DoubleAttribute m_fadeRatio;
    DoubleAttribute m_stepTime;
    DoubleAttribute m_timeVaration;

    HsvMatrix m_oldValues;
    HsvMatrix m_newValues;
    double m_currentStepTime;
	double m_pos;

};

#endif // RANDOMVALUEBLOCK_H
