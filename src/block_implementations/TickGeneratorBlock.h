#ifndef TICKGENERATORBLOCK_H
#define TICKGENERATORBLOCK_H

#include "block_data/InOutBlock.h"
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
        info.category << "Generator";
        info.helpText = "Continuously generates ticks with the specified duration between them.\n"
                        "Use it for example to trigger a GO button.";
        info.qmlFile = "qrc:/qml/Blocks/TickGeneratorBlock.qml";
        info.complete<TickGeneratorBlock>();
        return info;
    }

    explicit TickGeneratorBlock(MainController* controller, QString uid);

    QJsonObject getState() const override;
    void setState(const QJsonObject& state) override;

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
