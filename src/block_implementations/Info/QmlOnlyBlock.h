#ifndef QMLONLYBLOCK_H
#define QMLONLYBLOCK_H

#include "core/block_data/BlockBase.h"


class QmlOnlyBlock: public BlockBase
{
    Q_OBJECT

    Q_PROPERTY(QString qmlFilePath READ getQmlFilePath WRITE setQmlFilePath NOTIFY qmlFilePathChanged)
    Q_PROPERTY(QString fileBaseName READ getFileBaseName NOTIFY qmlFilePathChanged)

public:

    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "QML Only";
        info.nameInUi = "Custom QML File";
        info.category << "General" << "Other";
        info.helpText = "Loads a custom QML file.\n"
                        "Can be used to create own Blocks."
                        "Current Limitations: no node connections can be created "
                        "and that there is no way to save data.\n\n"
                        "For examples please contact me at mail@luminosus.org.";
        info.availabilityRequirements << AvailabilityRequirement::Scripting
                                      << AvailabilityRequirement::LocalFileSelection;
        info.qmlFile = "qrc:/qml/Blocks/Info/QmlOnlyBlock.qml";
        info.complete<QmlOnlyBlock>();
        return info;
    }

    explicit QmlOnlyBlock(MainController* controller, QString uid);

    virtual void getAdditionalState(QJsonObject& state) const override;
    virtual void setAdditionalState(const QJsonObject& state) override;

signals:
    void qmlFilePathChanged();

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    QString getQmlFilePath() const { return m_qmlFilePath; }
    void setQmlFilePath(QString value) { m_qmlFilePath = value; emit qmlFilePathChanged(); }

    QString getFileBaseName() const;

protected:
    QString m_qmlFilePath;
};

#endif // QMLONLYBLOCK_H
