#ifndef SCENESLIDERBLOCK_H
#define SCENESLIDERBLOCK_H

#include "core/block_data/BlockBase.h"


class SceneSliderBlock : public BlockBase
{
    Q_OBJECT

public:

    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Scene Slider";
        info.nameInUi = "Extra Scene Value";
        info.category << "Theater";
        info.helpText = "A simple slider that outputs values between 0 and 1 "
                        "and that saves its value to a scene.";
        info.visibilityRequirements << VisibilityRequirement::StandaloneVersion;
        info.qmlFile = "qrc:/qml/Blocks/Theater/SceneSliderBlock.qml";
        info.complete<SceneSliderBlock>();
        return info;
    }

    explicit SceneSliderBlock(MainController* controller, QString uid);

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    void update();

    virtual void addToBench(const SceneData& data) override;

    virtual void clearBench() override;

    virtual HsvMatrix getMixData() const override;
    virtual HsvMatrix getBenchData() const override;
    virtual void updateFromSceneData() override { update(); }

protected:
    QPointer<NodeBase> m_outputNode;

    DoubleAttribute m_benchValue;
    DoubleAttribute m_resultValue;
};

#endif // SCENESLIDERBLOCK_H
