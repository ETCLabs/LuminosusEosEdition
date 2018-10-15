#ifndef DIMMERBLOCK_H
#define DIMMERBLOCK_H

#include "core/block_data/FixtureBlock.h"
#include "core/SmartAttribute.h"
#include "core/Matrix.h"


class DimmerBlock : public FixtureBlock
{
    Q_OBJECT

public:

    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Dimmer";
        info.nameInUi = "White Light";
        info.category << "Theater";
        info.helpText = "Outputs a value on a specific DMX channel via rudimentary ArtNet.";
        info.visibilityRequirements << VisibilityRequirement::StandaloneVersion;
        info.qmlFile = "qrc:/qml/Blocks/Theater/DimmerBlock.qml";
        info.complete<DimmerBlock>();
        return info;
    }

    explicit DimmerBlock(MainController* controller, QString uid);

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    void onInputChanged();

    void update();

    void toggleBenchEffects();

    void adjustHueSat(double dx, double dy);

    void setBrightness(double value);

    virtual void addToBench(const SceneData& data) override;

    virtual void clearBench() override;

    virtual HsvMatrix getMixData() const override;
    virtual HsvMatrix getBenchData() const override;
    virtual void updateFromSceneData() override { update(); }

protected:

    RgbAttribute m_benchColor;
    RgbAttribute m_resultColor;
    DoubleAttribute m_benchEffects;
    DoubleAttribute m_resultEffects;
};

#endif // DIMMERBLOCK_H
