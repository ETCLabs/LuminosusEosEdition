#ifndef EOSSPEEDMASTERBLOCK_H
#define EOSSPEEDMASTERBLOCK_H

#include "core/block_data/OneInputBlock.h"
#include "core/SmartAttribute.h"
#include "utils.h"

#include <QTimer>


class EosSpeedMasterBlock : public OneInputBlock {

    Q_OBJECT

    Q_PROPERTY(QStringList effectNumbers READ getEffectNumbers NOTIFY effectNumbersChanged)

public:

    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Eos Speed Master";
        info.nameInUi = "Speed Master";
        info.category << "ETC Consoles" << "Eos";
        info.helpText = "Controls the speed of multiple Eos Effects.\n\n"
                        "All effects will be set to the same speed, except that "
                        "a multiplication factor can be set for each effect.\n\n"
                        "Connect a BPM Tap and BPM Detection Block to a Crossfade Block "
                        "and the Crossfade Block to this Speed Master to be able to "
                        "switch between the detected and tapped BPM easily.\n\n"
                        "This block uses Eos OSC user 0.";
        info.qmlFile = "qrc:/qml/Blocks/Eos/EosSpeedMasterBlock.qml";
        info.complete<EosSpeedMasterBlock>();
        return info;
    }

    explicit EosSpeedMasterBlock(MainController* controller, QString uid);

    virtual void getAdditionalState(QJsonObject& state) const override;
    virtual void setAdditionalState(const QJsonObject& state) override;

signals:
    void bpmChanged();
    void effectNumbersChanged();
    void multipliersChanged();

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    void onInputChanged();

    void addEffect(double effectNumber);

    void removeEffect(int index);

    void setMultiplier(int index, double newValue);

    double getMultiplier(int index) const;

    QStringList getEffectNumbers() const;

    double getBpm() const { return m_bpm; }

private slots:
    void sendBpm();

protected:
    DoubleAttribute m_bpm;

    QStringList m_effectNumbers;
    QVector<double> m_multipliers;

    BoolAttribute m_sendRegularly;
    QTimer m_timer;
};

#endif // EOSSPEEDMASTERBLOCK_H
