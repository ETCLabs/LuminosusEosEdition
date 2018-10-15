#ifndef EOSGROUPBLOCK_H
#define EOSGROUPBLOCK_H

#include "core/block_data/OneInputBlock.h"
#include "utils.h"
#include "core/Matrix.h"


class EosGroupBlock : public OneInputBlock
{
    Q_OBJECT

    Q_PROPERTY(int groupNumber READ getGroupNumber WRITE setGroupNumber NOTIFY groupNumberChanged)

public:

    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Eos Group";
        info.nameInUi = "Group";
        info.category << "ETC Consoles" << "Eos";
        info.helpText = "Controls the intensity and color of the selected group of an Eos console.\n\n"
                        "Sets OSC user implicitly to 0.";
        info.qmlFile = "qrc:/qml/Blocks/Eos/EosGroupBlock.qml";
        info.complete<EosGroupBlock>();
        return info;
    }

    explicit EosGroupBlock(MainController* controller, QString uid);

    virtual void getAdditionalState(QJsonObject& state) const override;
    virtual void setAdditionalState(const QJsonObject& state) override;

signals:
    void groupNumberChanged();

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    void onValueChanged();

    int getGroupNumber() const { return m_groupNumber; }
    void setGroupNumber(int value) { m_groupNumber = limit(1, value, 9999); emit groupNumberChanged(); }

protected:
    int m_groupNumber;
    HSV m_lastValue;
};


#endif // EOSGROUPBLOCK_H
