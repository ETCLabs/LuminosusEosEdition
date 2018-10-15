#ifndef TICKGENERATORBLOCK_H
#define TICKGENERATORBLOCK_H

#include "core/block_data/InOutBlock.h"
#include "utils.h"

#include <QtMath>


class TickGeneratorBlock : public InOutBlock
{
    Q_OBJECT

    Q_PROPERTY(double stepTime READ getStepTime WRITE setStepTime NOTIFY stepTimeChanged)

public:

    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Tick Generator";
        info.nameInUi = "Tick Generator";
        info.category << "General" << "Generator";
        info.helpText = "Continuously generates ticks with the specified duration between them. "
                        "For each tick the output is set to 1 for about 75ms.\n\n"
                        "Useful in connection with a BPM Detection block to generate a synthetic "
                        "perfect beat and trigger i.e. a button with every beat.";
        info.qmlFile = "qrc:/qml/Blocks/Logic/TickGeneratorBlock.qml";
        info.complete<TickGeneratorBlock>();
        return info;
    }

    explicit TickGeneratorBlock(MainController* controller, QString uid);

    void getAdditionalState(QJsonObject& state) const override;
    void setAdditionalState(const QJsonObject& state) override;

signals:
    void stepTimeChanged();

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    void onInputChanged();

    void sync();

    double getStepTime() const { return m_stepTime; }
    void setStepTime(double value) { m_stepTime = qMax(0.001, value); emit stepTimeChanged(); }

private slots:
    void eachFrame(double timeSinceLastFrame);

protected:
    double m_stepTime;

    double m_pos;
    int m_framesLeft;

};

#endif // TICKGENERATORBLOCK_H
