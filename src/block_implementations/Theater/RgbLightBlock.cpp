#include "RgbLightBlock.h"

#include "core/MainController.h"
#include "core/Nodes.h"


RgbLightBlock::RgbLightBlock(MainController *controller, QString uid)
    : FixtureBlock(controller, uid, /*footprint*/ 3)
    , m_benchColor(this, "benchColor", {0, 0, 0})
    , m_resultColor(this, "resultColor", {0, 0, 0})
    , m_benchEffects(this, "benchEffects", 0.0)
    , m_resultEffects(this, "resultEffects", 1.0)
    , m_pauseUpdate(false)
{
    m_address = m_controller->output()->getUnusedAddress(m_footprint);

    //connect signals and slots:
    connect(m_inputNode, SIGNAL(dataChanged()), this, SLOT(onInputChanged()));
    connect(&m_benchColor, SIGNAL(valueChanged()), this, SLOT(update()));
    connect(&m_benchEffects, SIGNAL(valueChanged()), this, SLOT(update()));
}

void RgbLightBlock::onInputChanged() {
    update();

    forwardDataToOutput();
}

void RgbLightBlock::update() {
    // Color attribute:
    double effects = m_benchEffects;
    RGB color = m_benchColor;
    for (const SceneData& data: m_sceneValues) {
        double factor = data.factor;
        effects = qMax(effects, data.values.at(0, 0).v * factor);
        HSV c = data.values.at(0, 1);
        // c is actually stored as RGB, so H is R, S is G and V is B:
        color.mixHtp({c.h * factor, c.s * factor, c.v * factor});
    }
    RGB effectColor = m_inputNode->isConnected() ? m_inputNode->constData().getRgbAt(0, 0) : RGB(0, 0, 0);
    m_resultEffects = effects;
    color.mixHtp(effectColor * effects);
    m_resultColor = color;

    // send color:
    m_controller->output()->setChannel(m_address, pow(m_resultColor.getValue().r, m_gamma));
    m_controller->output()->setChannel(m_address + 1, pow(m_resultColor.getValue().g, m_gamma));
    m_controller->output()->setChannel(m_address + 2, pow(m_resultColor.getValue().b, m_gamma));
}

void RgbLightBlock::toggleBenchEffects() {
    m_pauseUpdate = true;
    if (m_benchEffects > LuminosusConstants::triggerThreshold) {
        m_benchEffects = 0.0;
        RGB dynamicColor = m_inputNode->constData().getRgbAt(0, 0);
        m_benchColor = dynamicColor;
    } else {
        m_benchEffects = 1.0;
        m_benchColor = {0.0, 0.0, 0.0};
    }
    m_pauseUpdate = false;
    update();
}

void RgbLightBlock::adjustHueSat(double dx, double dy) {
    QColor col = m_benchColor.getQColor();
    double hue = col.hueF();
    double sat = col.saturationF();
    double val = col.valueF();
    double dp = m_controller->guiManager()->getGuiScaling();
    hue = realMod(hue + dx / (500.0*dp), 1.0);
    sat = limitToOne(sat + dy / (-300.0*dp));
    m_benchColor.setQColor(QColor::fromHsvF(hue, sat, val));
}

void RgbLightBlock::setBrightness(double value) {
    QColor col = m_benchColor.getQColor();
    double hue = col.hueF();
    double sat = col.saturationF();
    m_benchColor.setQColor(QColor::fromHsvF(hue, sat, limitToOne(value)));
}

void RgbLightBlock::addToBench(const SceneData& data) {
    m_pauseUpdate = true;
    double factor = data.factor;
    HSV c = data.values.at(0, 1);
    // c is actually stored as RGB, so H is R, S is G and V is B:
    m_benchColor.mixHtp({c.h * factor, c.s * factor, c.v * factor});
    m_benchEffects = qMax(m_benchEffects.getValue(), data.values.at(0, 0).v * factor);
    m_pauseUpdate = false;
    update();
}

void RgbLightBlock::clearBench() {
    m_pauseUpdate = true;
    m_benchColor.setValue({0, 0, 0});
    m_benchEffects.setValue(0.0);
    m_pauseUpdate = false;
    update();
}

HsvMatrix RgbLightBlock::getMixData() const {
    HsvMatrix matrix;
    matrix.rescale(1, 2);
    matrix.at(0, 0).v = m_resultEffects;

    // calculate result color without (!) input node (effect) color:
    RGB color = m_benchColor;
    for (const SceneData& data: m_sceneValues) {
        double factor = data.factor;
        HSV c = data.values.at(0, 1);
        // c is actually stored as RGB, so H is R, S is G and V is B:
        color.mixHtp({c.h * factor, c.s * factor, c.v * factor});
    }

    // color is actually stored as RGB, so H is R, S is G and V is B:
    matrix.at(0, 1) = HSV(color.r, color.g, color.b);
    return matrix;
}

HsvMatrix RgbLightBlock::getBenchData() const {
    HsvMatrix matrix;
    matrix.rescale(1, 2);
    matrix.at(0, 0).v = m_benchEffects;
    // color is actually stored as RGB, so H is R, S is G and V is B:
    matrix.at(0, 1) = HSV(m_benchColor.getValue().r, m_benchColor.getValue().g, m_benchColor.getValue().b);
    return matrix;
}
