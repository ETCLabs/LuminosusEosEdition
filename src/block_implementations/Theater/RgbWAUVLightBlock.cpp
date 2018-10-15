#include "RgbWAUVLightBlock.h"

#include "core/MainController.h"
#include "core/Nodes.h"


RgbWAUVLightBlock::RgbWAUVLightBlock(MainController *controller, QString uid)
    : FixtureBlock(controller, uid, /*footprint*/ 8)
    , m_inputNodeWhite(nullptr)
    , m_outputNodeWhite(nullptr)
    , m_inputNodeAmber(nullptr)
    , m_outputNodeAmber(nullptr)
    , m_inputNodeUV(nullptr)
    , m_outputNodeUV(nullptr)
    , m_benchColor(this, "benchColor", {0, 0, 0})
    , m_resultColor(this, "resultColor", {0, 0, 0})
    , m_benchWhite(this, "benchWhite", 0)
    , m_resultWhite(this, "resultWhite", 0)
    , m_benchAmber(this, "benchAmber", 0)
    , m_resultAmber(this, "resultAmber", 0)
    , m_benchUV(this, "benchUV", 0)
    , m_resultUV(this, "resultUV", 0)
    , m_benchEffects(this, "benchEffects", 0.0)
    , m_resultEffects(this, "resultEffects", 1.0)
    , m_pauseUpdate(false)
{
    m_address = m_controller->output()->getUnusedAddress(m_footprint);
    updateBrightnessChannel();

    m_inputNodeWhite = createInputNode("whiteIn");
    m_outputNodeWhite = createOutputNode("whiteOut");
    m_inputNodeAmber = createInputNode("amberIn");
    m_outputNodeAmber = createOutputNode("amberOut");
    m_inputNodeUV = createInputNode("uvIn");
    m_outputNodeUV = createOutputNode("uvOut");

    //connect signals and slots:
    connect(m_inputNode, SIGNAL(dataChanged()), this, SLOT(onInputChanged()));
    connect(m_inputNodeWhite, SIGNAL(dataChanged()), this, SLOT(onInputChangedWhite()));
    connect(m_inputNodeAmber, SIGNAL(dataChanged()), this, SLOT(onInputChangedAmber()));
    connect(m_inputNodeUV, SIGNAL(dataChanged()), this, SLOT(onInputChangedUV()));
    connect(&m_benchColor, SIGNAL(valueChanged()), this, SLOT(update()));
    connect(&m_benchWhite, SIGNAL(valueChanged()), this, SLOT(update()));
    connect(&m_benchAmber, SIGNAL(valueChanged()), this, SLOT(update()));
    connect(&m_benchUV, SIGNAL(valueChanged()), this, SLOT(update()));
    connect(&m_benchEffects, SIGNAL(valueChanged()), this, SLOT(update()));

    connectSlots(m_inputNodeWhite, m_outputNodeWhite);
    connectSlots(m_inputNodeAmber, m_outputNodeAmber);
    connectSlots(m_inputNodeUV, m_outputNodeUV);

    connect(&m_address, SIGNAL(valueChanged()), this, SLOT(updateBrightnessChannel()));
}

void RgbWAUVLightBlock::onInputChanged() {
    update();
    forwardDataToOutput();
}

void RgbWAUVLightBlock::onInputChangedWhite() {
    update();
    forwardData(m_inputNodeWhite, m_outputNodeWhite);
}

void RgbWAUVLightBlock::onInputChangedAmber() {
    update();
    forwardData(m_inputNodeAmber, m_outputNodeAmber);
}

void RgbWAUVLightBlock::onInputChangedUV() {
    update();
    forwardData(m_inputNodeUV, m_outputNodeUV);
}

void RgbWAUVLightBlock::update() {
    if (m_pauseUpdate) return;
    // Color attribute:
    double effects = m_benchEffects;
    RGB color = m_benchColor;
    double white = m_benchWhite;
    double amber = m_benchAmber;
    double uv = m_benchUV;
    for (const SceneData& data: m_sceneValues) {
        double factor = data.factor;
        effects = qMax(effects, data.values.at(0, 0).v * factor);
        white = qMax(white, data.values.at(0, 2).v * factor);
        amber = qMax(amber, data.values.at(0, 3).v * factor);
        uv = qMax(uv, data.values.at(0, 4).v * factor);
        HSV c = data.values.at(0, 1);
        // c is actually stored as RGB, so H is R, S is G and V is B:
        color.mixHtp({c.h * factor, c.s * factor, c.v * factor});
    }
    const RGB effectColor = m_inputNode->isConnected() ? m_inputNode->constData().getRgbAt(0, 0) : RGB(0, 0, 0);
    const double effectWhite = m_inputNodeWhite->isConnected() ? m_inputNodeWhite->getValue() : 0.0;
    const double effectAmber = m_inputNodeAmber->isConnected() ? m_inputNodeAmber->getValue() : 0.0;
    const double effectUV = m_inputNodeUV->isConnected() ? m_inputNodeUV->getValue() : 0.0;
    m_resultEffects = effects;
    m_resultWhite = qMax(white, effectWhite * effects);
    m_resultAmber = qMax(amber, effectAmber * effects);
    m_resultUV = qMax(uv, effectUV * effects);
    color.mixHtp(effectColor * effects);
    m_resultColor = color;

    // send color:
    m_controller->output()->setChannel(m_address + 2, pow(m_resultColor.getValue().r, m_gamma));
    m_controller->output()->setChannel(m_address + 3, pow(m_resultColor.getValue().g, m_gamma));
    m_controller->output()->setChannel(m_address + 4, pow(m_resultColor.getValue().b, m_gamma));
    m_controller->output()->setChannel(m_address + 5, pow(m_resultWhite, m_gamma));
    m_controller->output()->setChannel(m_address + 6, pow(m_resultAmber, m_gamma));
    m_controller->output()->setChannel(m_address + 7, pow(m_resultUV, m_gamma));
}

void RgbWAUVLightBlock::updateBrightnessChannel() {
    m_controller->output()->setChannel(m_address, 1.0);
    m_controller->output()->setChannel(m_address + 1, 0.0);
}

void RgbWAUVLightBlock::toggleBenchEffects() {
    m_pauseUpdate = true;
    if (m_benchEffects > LuminosusConstants::triggerThreshold) {
        m_benchEffects = 0.0;
        RGB dynamicColor = m_inputNode->constData().getRgbAt(0, 0);
        m_benchColor = dynamicColor;
        m_benchWhite = m_inputNodeWhite->getValue();
        m_benchAmber = m_inputNodeAmber->getValue();
        m_benchUV = m_inputNodeUV->getValue();
    } else {
        m_benchEffects = 1.0;
        m_benchColor = {0.0, 0.0, 0.0};
        m_benchWhite = 0.0;
        m_benchAmber = 0.0;
        m_benchUV = 0.0;
    }
    m_pauseUpdate = false;
    update();
}

void RgbWAUVLightBlock::adjustHueSat(double dx, double dy) {
    QColor col = m_benchColor.getQColor();
    double hue = col.hueF();
    double sat = col.saturationF();
    double val = col.valueF();
    double dp = m_controller->guiManager()->getGuiScaling();
    hue = realMod(hue + dx / (500.0*dp), 1.0);
    sat = limitToOne(sat + dy / (-300.0*dp));
    m_benchColor.setQColor(QColor::fromHsvF(hue, sat, val));
}

void RgbWAUVLightBlock::setBrightness(double value) {
    QColor col = m_benchColor.getQColor();
    double hue = col.hueF();
    double sat = col.saturationF();
    m_benchColor.setQColor(QColor::fromHsvF(hue, sat, limitToOne(value)));
}

void RgbWAUVLightBlock::addToBench(const SceneData& data) {
    m_pauseUpdate = true;
    double factor = data.factor;
    HSV c = data.values.at(0, 1);
    // c is actually stored as RGB, so H is R, S is G and V is B:
    m_benchColor.mixHtp({c.h * factor, c.s * factor, c.v * factor});
    m_benchEffects = qMax(m_benchEffects.getValue(), data.values.at(0, 0).v * factor);
    m_benchWhite = qMax(m_benchWhite.getValue(), data.values.at(0, 2).v * factor);
    m_benchAmber = qMax(m_benchAmber.getValue(), data.values.at(0, 3).v * factor);
    m_benchUV = qMax(m_benchUV.getValue(), data.values.at(0, 4).v * factor);
    m_pauseUpdate = false;
    update();
}

void RgbWAUVLightBlock::clearBench() {
    m_pauseUpdate = true;
    m_benchColor = {0, 0, 0};
    m_benchEffects = 0.0;
    m_benchWhite = 0.0;
    m_benchAmber = 0.0;
    m_benchUV = 0.0;
    m_pauseUpdate = false;
    update();
}

HsvMatrix RgbWAUVLightBlock::getMixData() const {
    HsvMatrix matrix;
    matrix.rescale(1, 5);
    matrix.at(0, 0).v = m_resultEffects;

    // calculate result colors without (!) input node (effect) colors:
    RGB color = m_benchColor;
    double white = m_benchWhite;
    double amber = m_benchAmber;
    double uv = m_benchUV;
    for (const SceneData& data: m_sceneValues) {
        double factor = data.factor;
        white = qMax(white, data.values.at(0, 2).v * factor);
        amber = qMax(amber, data.values.at(0, 3).v * factor);
        uv = qMax(uv, data.values.at(0, 4).v * factor);
        HSV c = data.values.at(0, 1);
        // c is actually stored as RGB, so H is R, S is G and V is B:
        color.mixHtp({c.h * factor, c.s * factor, c.v * factor});
    }

    // color is actually stored as RGB, so H is R, S is G and V is B:
    matrix.at(0, 1) = HSV(color.r, color.g, color.b);
    matrix.at(0, 2).v = white;
    matrix.at(0, 3).v = amber;
    matrix.at(0, 4).v = uv;
    return matrix;
}

HsvMatrix RgbWAUVLightBlock::getBenchData() const {
    HsvMatrix matrix;
    matrix.rescale(1, 5);
    matrix.at(0, 0).v = m_benchEffects;
    // color is actually stored as RGB, so H is R, S is G and V is B:
    matrix.at(0, 1) = HSV(m_benchColor.getValue().r, m_benchColor.getValue().g, m_benchColor.getValue().b);
    matrix.at(0, 2).v = m_benchWhite;
    matrix.at(0, 3).v = m_benchAmber;
    matrix.at(0, 4).v = m_benchUV;
    return matrix;
}
