#ifndef SINUSVALUEBLOCK_H
#define SINUSVALUEBLOCK_H

#include "core/block_data/InOutBlock.h"
#include "utils.h"

#include <QtMath>


class SinusValueBlock : public InOutBlock
{
    Q_OBJECT

    Q_PROPERTY(double stepTime READ getStepTime WRITE setStepTime NOTIFY stepTimeChanged)

public:

    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Sinus Value";
        info.nameInUi = "Sinus Generator";
        info.category << "General" << "Generator";
        info.helpText = "Continuously outputs a sinus value between 0 and 1.";
        info.qmlFile = "qrc:/qml/Blocks/Logic/SinusValueBlock.qml";
        info.complete<SinusValueBlock>();
        return info;
    }

    explicit SinusValueBlock(MainController* controller, QString uid);

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

};

#endif // SINUSVALUEBLOCK_H
