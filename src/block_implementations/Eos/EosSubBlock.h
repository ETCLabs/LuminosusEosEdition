#ifndef EOSSUBBLOCK_H
#define EOSSUBBLOCK_H

#include "core/block_data/OneInputBlock.h"
#include "utils.h"


class EosSubBlock : public OneInputBlock
{
    Q_OBJECT

    Q_PROPERTY(int subNumber READ getSubNumber WRITE setSubNumber NOTIFY subNumberChanged)

    Q_PROPERTY(double value READ getValue WRITE setValue NOTIFY valueChanged)

public:

    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Eos Sub";
        info.nameInUi = "Sub";
        info.category << "ETC Consoles" << "Eos";
        info.helpText = "Controls the selected submaster of an Eos console.";
        info.qmlFile = "qrc:/qml/Blocks/Eos/EosSubBlock.qml";
        info.complete<EosSubBlock>();
        return info;
    }

    explicit EosSubBlock(MainController* controller, QString uid);

    virtual void getAdditionalState(QJsonObject& state) const override;
    virtual void setAdditionalState(const QJsonObject& state) override;

    // block needs to be rendered for MIDI mapping to work
    virtual bool renderIfNotVisible() const override { return true; }

signals:
    void subNumberChanged();
    void valueChanged();

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    void onValueChanged();

    void sendBumpEvent(double edge);

    int getSubNumber() const { return m_subNumber; }
    void setSubNumber(int value) { m_subNumber = limit(1, value, 32768); emit subNumberChanged(); }

    double getValue() const { return m_value; }
    void setValue(double value);

protected:
    int m_subNumber;
    double m_value;
};

#endif // EOSSUBBLOCK_H
