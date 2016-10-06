#ifndef EOSEFFECTBLOCK_H
#define EOSEFFECTBLOCK_H

#include "block_data/OneInputBlock.h"
#include "utils.h"


class EosEffectBlock : public OneInputBlock
{
    Q_OBJECT

    Q_PROPERTY(double effectNumber READ getEffectNumber WRITE setEffectNumber NOTIFY effectNumberChanged)

public:

    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Eos Effect";
        info.nameInUi = "Effect Speed";
        info.category << "Eos";
        info.helpText = "Controls the speed of the selected effect of an Eos console.\n"
                        "Sets OSC user implicitly to 0.";
        info.qmlFile = "qrc:/qml/Blocks/EosEffectBlock.qml";
        info.complete<EosEffectBlock>();
        return info;
    }

    explicit EosEffectBlock(MainController* controller, QString uid);

    virtual QJsonObject getState() const override;
    virtual void setState(const QJsonObject& state) override;

signals:
    void effectNumberChanged();

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    void onValueChanged();

    double getEffectNumber() const { return m_effectNumber; }
    void setEffectNumber(double value) { m_effectNumber = limit(1, value, 9999); emit effectNumberChanged(); }

protected:
    double m_effectNumber;
    double m_lastValue;
};

#endif // EOSEFFECTBLOCK_H
