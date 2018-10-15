#ifndef EOSEFFECTBLOCK_H
#define EOSEFFECTBLOCK_H

#include "core/block_data/OneInputBlock.h"
#include "utils.h"

#include <QTimer>


class EosEffectBlock : public OneInputBlock
{
    Q_OBJECT

    Q_PROPERTY(double effectNumber READ getEffectNumber WRITE setEffectNumber NOTIFY effectNumberChanged)

public:

    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Eos Effect";
        info.nameInUi = "Effect Speed";
        info.category << "ETC Consoles" << "Eos";
        info.helpText = "Controls the speed of the selected effect of an Eos console.\n"
                        "The input should be a time value, i.e. from a 'BPM Detection' block.\n\n"
                        "Sets OSC user implicitly to 0.";
        info.qmlFile = "qrc:/qml/Blocks/Eos/EosEffectBlock.qml";
        info.complete<EosEffectBlock>();
        return info;
    }

    explicit EosEffectBlock(MainController* controller, QString uid);

    virtual void getAdditionalState(QJsonObject& state) const override;
    virtual void setAdditionalState(const QJsonObject& state) override;

signals:
    void effectNumberChanged();

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    void onValueChanged();

    double getEffectNumber() const { return m_effectNumber; }
    void setEffectNumber(double value) { m_effectNumber = limit(1, value, 9999); emit effectNumberChanged(); }

private slots:
    void sendBpm();

protected:
    double m_effectNumber;
    double m_lastValue;

    BoolAttribute m_sendRegularly;
    QTimer m_timer;
};

#endif // EOSEFFECTBLOCK_H
