#ifndef LINEARVALUEBLOCK_H
#define LINEARVALUEBLOCK_H

#include "block_data/InOutBlock.h"
#include "utils.h"

#include <QtMath>


class LinearValueBlock : public InOutBlock
{
    Q_OBJECT

    Q_PROPERTY(double stepTime READ getStepTime WRITE setStepTime NOTIFY stepTimeChanged)

public:

    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Linear Value";
        info.nameInUi = "Sawtooth Generator";
        info.category << "Generator";
        info.helpText = "Continuously outputs a value linearly increasing from 0 to 1.\n"
                        "(Sawtooth)";
        info.qmlFile = "qrc:/qml/Blocks/LinearValueBlock.qml";
        info.complete<LinearValueBlock>();
        return info;
    }

    explicit LinearValueBlock(MainController* controller, QString uid);

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

};

#endif // LINEARVALUEBLOCK_H
