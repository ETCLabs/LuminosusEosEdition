#ifndef RGBWAUVLIGHTBLOCK_H
#define RGBWAUVLIGHTBLOCK_H

#include "core/block_data/FixtureBlock.h"
#include "core/SmartAttribute.h"
#include "core/Matrix.h"


class RgbWAUVLightBlock : public FixtureBlock
{
    Q_OBJECT

public:

    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "RGB-W-A-UV Light";
        info.nameInUi = "RGB-W-A-UV Light";
        info.category << "Theater";
        info.helpText = "Outputs a RGB, White, Amber and UV value via rudimentary ArtNet.";
        info.visibilityRequirements << VisibilityRequirement::StandaloneVersion;
        info.qmlFile = "qrc:/qml/Blocks/Theater/RgbWAUVLightBlock.qml";
        info.complete<RgbWAUVLightBlock>();
        return info;
    }

    explicit RgbWAUVLightBlock(MainController* controller, QString uid);

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    void onInputChanged();
    void onInputChangedWhite();
    void onInputChangedAmber();
    void onInputChangedUV();

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
    QPointer<NodeBase> m_inputNodeAmber;
    QPointer<NodeBase> m_outputNodeAmber;
    QPointer<NodeBase> m_inputNodeUV;
    QPointer<NodeBase> m_outputNodeUV;

    RgbAttribute m_benchColor;
    RgbAttribute m_resultColor;

    DoubleAttribute m_benchWhite;
    DoubleAttribute m_resultWhite;
    DoubleAttribute m_benchAmber;
    DoubleAttribute m_resultAmber;
    DoubleAttribute m_benchUV;
    DoubleAttribute m_resultUV;

    DoubleAttribute m_benchEffects;
    DoubleAttribute m_resultEffects;

    bool m_pauseUpdate;
};

#endif // RGBWAUVLIGHTBLOCK_H
