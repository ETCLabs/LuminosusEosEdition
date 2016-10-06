#ifndef EOSGROUPBLOCK_H
#define EOSGROUPBLOCK_H

#include "block_data/OneInputBlock.h"
#include "utils.h"


class EosGroupBlock : public OneInputBlock
{
    Q_OBJECT

    Q_PROPERTY(int groupNumber READ getGroupNumber WRITE setGroupNumber NOTIFY groupNumberChanged)

public:

    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Eos Group";
        info.nameInUi = "Group";
        info.category << "Eos";
        info.helpText = "Controls the selected group of an Eos console.\n"
                        "Sets OSC user implicitly to 0.";
        info.qmlFile = "qrc:/qml/Blocks/EosGroupBlock.qml";
        info.complete<EosGroupBlock>();
        return info;
    }

    explicit EosGroupBlock(MainController* controller, QString uid);

    virtual QJsonObject getState() const override;
    virtual void setState(const QJsonObject& state) override;

signals:
    void groupNumberChanged();

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    void onValueChanged();

    int getGroupNumber() const { return m_groupNumber; }
    void setGroupNumber(int value) { m_groupNumber = limit(1, value, 9999); emit groupNumberChanged(); }

protected:
    int m_groupNumber;
    std::vector<double> m_lastValue;
};


#endif // EOSGROUPBLOCK_H
