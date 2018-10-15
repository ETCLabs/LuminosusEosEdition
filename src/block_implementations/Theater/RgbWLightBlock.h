#ifndef RGBWLIGHTBLOCK_H
#define RGBWLIGHTBLOCK_H

#include "core/block_data/FixtureBlock.h"
#include "core/SmartAttribute.h"
#include "core/Matrix.h"


class RgbWLightBlock : public FixtureBlock
{
    Q_OBJECT

public:

    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "RGB-W Light";
        info.nameInUi = "RGB-W Light";
        info.category << "Theater";
        info.helpText = "Outputs a RGB and White value via rudimentary ArtNet.";
        info.visibilityRequirements << VisibilityRequirement::StandaloneVersion;
        info.qmlFile = "qrc:/qml/Blocks/Theater/RgbWLightBlock.qml";
        info.complete<RgbWLightBlock>();
        return info;
    }

    explicit RgbWLightBlock(MainController* controller, QString uid);

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    void onInputChanged();
    void onInputChangedWhite();

    void update();

    void updateBrightnessChannel();

    void toggleBenchEffects();

    void adjustHueSat(double dx, double dy);

    void setBrightness(double value);

    virtual void addToBench(const SceneData& data) override;

    virtual void clearBench() override;

    virtual HsvMatrix getMixData() const override;
    virtual HsvMatrix getBenchData() const override;
    virtual void updateFromSceneData() override { update(); }

protected:
    QPointer<NodeBase> m_inputNodeWhite;
    QPointer<NodeBase> m_outputNodeWhite;

    RgbAttribute m_benchColor;
    RgbAttribute m_resultColor;

    DoubleAttribute m_benchWhite;
    DoubleAttribute m_resultWhite;

    DoubleAttribute m_benchEffects;
    DoubleAttribute m_resultEffects;

    bool m_pauseUpdate;
};

#endif // RGBWLIGHTBLOCK_H
