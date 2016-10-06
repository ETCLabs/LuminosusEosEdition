#ifndef EOSCUELISTBLOCK_H
#define EOSCUELISTBLOCK_H

#include "block_data/BlockBase.h"
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
        info.category << "Eos";
        info.helpText = "Shows the selected Cue List.\n\n"
                        "A Cue can be edited by clicking on the pen icon. "
                        "The changes will be transmitted to the console after a click on the "
                        "green tick icon.";
        info.qmlFile = "qrc:/qml/Blocks/EosCueListBlock.qml";
        info.complete<EosCueListBlock>();
        return info;
    }

    explicit EosCueListBlock(MainController* controller, QString uid);

    virtual QJsonObject getState() const override;
    virtual void setState(const QJsonObject& state) override;

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
