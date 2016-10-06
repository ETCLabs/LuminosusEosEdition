#ifndef EOSMACROBLOCK_H
#define EOSMACROBLOCK_H

#include "block_data/OneInputBlock.h"
#include "utils.h"


class EosMacroBlock : public OneInputBlock
{
    Q_OBJECT

    Q_PROPERTY(int macroNumber READ getMacroNumber WRITE setMacroNumber NOTIFY macroNumberChanged)

public:

    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Eos Macro";
        info.nameInUi = "Macro";
        info.category << "Eos";
        info.helpText = "Fires the selected Macro.";
        info.qmlFile = "qrc:/qml/Blocks/EosMacroBlock.qml";
        info.complete<EosMacroBlock>();
        return info;
    }

    explicit EosMacroBlock(MainController* controller, QString uid);

    virtual QJsonObject getState() const override;
    virtual void setState(const QJsonObject& state) override;

signals:
    void macroNumberChanged();

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    void onValueChanged();

    void fire(double edge);

    int getMacroNumber() const { return m_macroNumber; }
    void setMacroNumber(int value) { m_macroNumber = limit(1, value, 9999); emit macroNumberChanged(); }

protected:
    int m_macroNumber;
    double m_lastValue;
};


#endif // EOSMACROBLOCK_H
