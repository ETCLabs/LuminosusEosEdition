#ifndef EOSCMDBLOCK_H
#define EOSCMDBLOCK_H

#include "core/block_data/BlockBase.h"
#include "utils.h"


class EosCmdBlock : public BlockBase
{
    Q_OBJECT

    Q_PROPERTY(int userId READ getUserId WRITE setUserId NOTIFY userIdChanged)

public:

    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Eos Cmd";
        info.nameInUi = "Commandline";
        info.category << "ETC Consoles" << "Eos";
        info.helpText = "Shows the current commandline text of the selected Eos user.";
        info.qmlFile = "qrc:/qml/Blocks/Eos/EosCmdBlock.qml";
        info.complete<EosCmdBlock>();
        return info;
    }

    explicit EosCmdBlock(MainController* controller, QString uid);

    virtual void getAdditionalState(QJsonObject& state) const override;
    virtual void setAdditionalState(const QJsonObject& state) override;

signals:
    void userIdChanged();

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    int getUserId() const { return m_userId; }
    void setUserId(int value) { m_userId = limit(0, value, 999); emit userIdChanged(); }

protected:
    int m_userId;
};

#endif // EOSCMDBLOCK_H
