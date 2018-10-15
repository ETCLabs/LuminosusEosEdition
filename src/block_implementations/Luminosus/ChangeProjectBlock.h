#ifndef CHANGEPROJECTBLOCK_H
#define CHANGEPROJECTBLOCK_H

#include "core/block_data/OneInputBlock.h"

class ChangeProjectBlock : public OneInputBlock
{
    Q_OBJECT

    Q_PROPERTY(QString projectName READ getProjectName WRITE setProjectName NOTIFY projectNameChanged)
    Q_PROPERTY(bool animated READ getAnimated WRITE setAnimated NOTIFY animatedChanged)

public:

    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Change Project";
        info.nameInUi = "Change Project";
        info.category << "General" << "Groups / Projects";
        info.helpText = "Opens the selected project.\n\n"
                        "Useful to assign buttons to open projects, but remember "
                        "to assign them also in the other direction.";
        info.qmlFile = "qrc:/qml/Blocks/Luminosus/ChangeProjectBlock.qml";
        info.complete<ChangeProjectBlock>();
        return info;
    }

    explicit ChangeProjectBlock(MainController* controller, QString uid);

    virtual void getAdditionalState(QJsonObject& state) const override;
    virtual void setAdditionalState(const QJsonObject& state) override;

signals:
    void projectNameChanged();
    void animatedChanged();

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    void onInputChanged();

    void fire();

    QString getProjectName() const { return m_projectName; }
    void setProjectName(const QString& value) { m_projectName = value; emit projectNameChanged(); }

    bool getAnimated() const { return m_animated; }
    void setAnimated(bool value) { m_animated = value; emit animatedChanged(); }

protected:
    QString m_projectName;
    bool m_animated;
    double m_lastValue;
};

#endif // CHANGEPROJECTBLOCK_H
