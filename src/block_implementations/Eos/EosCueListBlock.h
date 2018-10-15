#ifndef EOSCUELISTBLOCK_H
#define EOSCUELISTBLOCK_H

#include "core/block_data/BlockBase.h"
#include "utils.h"


class EosCueListBlock: public BlockBase
{
    Q_OBJECT

    Q_PROPERTY(int cueListNumber READ getCueListNumber WRITE setCueListNumber NOTIFY cueListNumberChanged)

public:

    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Eos CueList";
        info.nameInUi = "Cue List";
        info.category << "ETC Consoles" << "Eos";
        info.helpText = "Shows the selected Cue List.\n\n"
                        "A Cue can be edited by clicking on the pen icon. "
                        "The changes will be transmitted to the console after a click on the "
                        "green tick icon.";
        info.qmlFile = "qrc:/qml/Blocks/Eos/EosCueListBlock.qml";
        info.complete<EosCueListBlock>();
        return info;
    }

    explicit EosCueListBlock(MainController* controller, QString uid);

    virtual void getAdditionalState(QJsonObject& state) const override;
    virtual void setAdditionalState(const QJsonObject& state) override;

signals:
    void cueListNumberChanged();

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    int getCueListNumber() const { return m_cueListNumber; }
    void setCueListNumber(int value) { m_cueListNumber = limit(1, value, 999); emit cueListNumberChanged(); }

protected:
    int m_cueListNumber;

};

#endif // EOSCUELISTBLOCK_H
