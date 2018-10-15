#ifndef EOSMACROBLOCK_H
#define EOSMACROBLOCK_H

#include "core/block_data/OneInputBlock.h"
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
        info.category << "ETC Consoles" << "Eos";
        info.helpText = "Fires the selected Macro.\n\n"
                        "It uses Eos OSC user 0 if 'run in background' is enabled, otherwise user 1.";
        info.qmlFile = "qrc:/qml/Blocks/Eos/EosMacroBlock.qml";
        info.complete<EosMacroBlock>();
        return info;
    }

    explicit EosMacroBlock(MainController* controller, QString uid);

    virtual void getAdditionalState(QJsonObject& state) const override;
    virtual void setAdditionalState(const QJsonObject& state) override;

    // block needs to be rendered for MIDI mapping to work
    virtual bool renderIfNotVisible() const override { return true; }

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
    BoolAttribute m_runInbackground;
};


#endif // EOSMACROBLOCK_H
