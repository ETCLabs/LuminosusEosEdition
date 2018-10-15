#ifndef PRESETBLOCK_H
#define PRESETBLOCK_H

#include "core/block_data/BlockBase.h"
#include "core/SmartAttribute.h"
#include "utils.h"

#include <QMap>
#include <QPointer>


class PresetBlock : public BlockBase
{
    Q_OBJECT

public:

    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Preset";
        info.nameInUi = "Preset";
        info.category << "Theater";
        info.helpText = "";
        info.visibilityRequirements << VisibilityRequirement::StandaloneVersion;
        info.qmlFile = "qrc:/qml/Blocks/Theater/PresetBlock.qml";
        info.complete<PresetBlock>();
        return info;
    }

    explicit PresetBlock(MainController* controller, QString uid);
    virtual ~PresetBlock() override;

    friend class CueListBlock;

    virtual void getAdditionalState(QJsonObject& state) const override;
    virtual void setAdditionalState(const QJsonObject& state) override;

    virtual bool mayBeRemoved() override;

signals:
    void blockIsVisibleChanged();

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    void saveFromMix();

    void saveFromBenches();

    void pushToBanks();

    void addToCueList();

    void releaseIfHidden();

    QMap<QPointer<BlockInterface>, HsvMatrix>* getSceneData() { return &m_sceneData; }

private slots:
    void update();

    void convertPersistentSceneData();

protected:
    DoubleAttribute m_value;

    QMap<QPointer<BlockInterface>, HsvMatrix> m_sceneData;
    QMap<QString, HsvMatrix> m_persistentSceneData;


};

#endif // PRESETBLOCK_H
