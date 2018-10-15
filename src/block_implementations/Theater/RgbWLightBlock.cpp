#include "RgbWLightBlock.h"

#include "core/MainController.h"
#include "core/Nodes.h"


RgbWLightBlock::RgbWLightBlock(MainController *controller, QString uid)
    : FixtureBlock(controller, uid, /*footprint*/ 3)
    , m_inputNodeWhite(nullptr)
    , m_outputNodeWhite(nullptr)
    , m_benchColor(this, "benchColor", {0, 0, 0})
    , m_resultColor(this, "resultColor", {0, 0, 0})
    , m_benchWhite(this, "benchWhite", 0)
    , m_resultWhite(this, "resultWhite", 0)
    , m_benchEffects(this, "benchEffects", 0.0)
    , m_resultEffects(this, "resultEffects", 1.0)
    , m_pauseUpdate(false)
{
    m_address = m_controller->output()->getUnusedAddress(m_footprint);

    m_inputNodeWhite = createInputNode("whiteIn");
    m_outputNodeWhite = createOutputNode("whiteOut");

    //connect signals and slots:
    connect(m_inputNode, SIGNAL(dataChanged()), this, SLOT(onInputChanged()));
    connect(m_inputNodeWhite, SIGNAL(dataChanged()), this, SLOT(onInputChangedWhite()));
    connect(&m_benchColor, SIGNAL(valueChanged()), this, SLOT(update()));
    connect(&m_benchWhite, SIGNAL(valueChanged()), this, SLOT(update()));
    connect(&m_benchEffects, SIGNAL(valueChanged()), this, SLOT(update()));

    connectSlots(m_inputNodeWhite, m_outputNodeWhite);

    connect(&m_address, SIGNAL(valueChanged()), this, SLOT(updateBrightnessChannel()));
}

void RgbWLightBlock::onInputChanged() {
    update();
    forwardDataToOutput();
}

void RgbWLightBlock::onInputChangedWhite() {
    update();
    forwardData(m_inputNodeWhite, m_outputNodeWhite);
}

void RgbWLightBlock::update() {
    // Color attribute:
    double effects = m_benchEffects;
    RGB color = m_benchColor;
    double white = m_benchWhite;
    for (const SceneData& data: m_sceneValues) {
        double factor = data.factor;
        effects = qMax(effects, data.values.at(0, 0).v * factor);
        white = qMax(white, data.values.at(0, 2).v * factor);
        HSV c = data.values.at(0, 1);
        // c is actually stored as RGB, so H is R, S is G and V is B:
        color.mixHtp({c.h * factor, c.s * factor, c.v * factor});
    }
    const RGB effectColor = m_inputNode->isConnected() ? m_inputNode->constData().getRgbAt(0, 0) : RGB(0, 0, 0);
    const double effectWhite = m_inputNodeWhite->isConnected() ? m_inputNodeWhite->getValue() : 0.0;
    m_resultEffects = effects;
    m_resultWhite = qMax(white, effectWhite * effects);
    color.mixHtp(effectColor * effects);
    m_resultColor = color;

    // send color:
    m_controller->output()->setChannel(m_address, pow(m_resultColor.getValue().r, m_gamma));
    m_controller->output()->setChannel(m_address + 1, pow(m_resultColor.getValue().g, m_gamma));
    m_controller->output()->setChannel(m_address + 2, pow(m_resultColor.getValue().b, m_gamma));
    m_controller->output()->setChannel(m_address + 3, pow(m_resultWhite, m_gamma));
}

void RgbWLightBlock::updateBrightnessChannel() {
    // TODO: make brightness channel configurable
    //m_controller->output()->setChannel(m_address , 1.0);
}

void RgbWLightBlock::toggleBenchEffects() {
    m_pauseUpdate = true;
    if (m_benchEffects > LuminosusConstants::triggerThreshold) {
        m_benchEffects = 0.0;
        RGB dynamicColor = m_inputNode->constData().getRgbAt(0, 0);
        m_benchColor = dynamicColor;
        m_benchWhite = m_inputNodeWhite->getValue();
    } else {
        m_benchEffects = 1.0;
        m_benchColor = {0.0, 0.0, 0.0};
        m_benchWhite = 0.0;
    }
    m_pauseUpdate = false;
    update();
}

void RgbWLightBlock::adjustHueSat(double dx, double dy) {
    QColor col = m_benchColor.getQColor();
    double hue = col.hueF();
    double sat = col.saturationF();
    double val = col.valueF();
    double dp = m_controller->guiManager()->getGuiScaling();
    hue = realMod(hue + dx / (500.0*dp), 1.0);
    sat = limitToOne(sat + dy / (-300.0*dp));
    m_benchColor.setQColor(QColor::fromHsvF(hue, sat, val));
}

void RgbWLightBlock::setBrightness(double value) {
    QColor col = m_benchColor.getQColor();
    double hue = col.hueF();
    double sat = col.saturationF();
    m_benchColor.setQColor(QColor::fromHsvF(hue, sat, limitToOne(value)));
}

void RgbWLightBlock::addToBench(const SceneData& data) {
    m_pauseUpdate = true;
    double factor = data.factor;
    HSV c = data.values.at(0, 1);
    // c is actually stored as RGB, so H is R, S is G and V is B:
    m_benchColor.mixHtp({c.h * factor, c.s * factor, c.v * factor});
    m_benchEffects = qMax(m_benchEffects.getValue(), data.values.at(0, 0).v * factor);
    m_benchWhite = qMax(m_benchWhite.getValue(), data.values.at(0, 2).v * factor);
    m_pauseUpdate = false;
    update();
}

void RgbWLightBlock::clearBench() {
    m_pauseUpdate = true;
    m_benchColor = {0, 0, 0};
    m_benchEffects = 0.0;
    m_benchWhite = 0.0;
    m_pauseUpdate = false;
    update();
}

HsvMatrix RgbWLightBlock::getMixData() const {
    HsvMatrix matrix;
    matrix.rescale(1, 3);
    matrix.at(0, 0).v = m_resultEffects;

    // calculate result colors without (!) input node (effect) colors:
    RGB color = m_benchColor;
    double white = m_benchWhite;
    for (const SceneData& data: m_sceneValues) {
        double factor = data.factor;
        white = qMax(white, data.values.at(0, 2).v * factor);
        HSV c = data.values.at(0, 1);
        // c is actually stored as RGB, so H is R, S is G and V is B:
        color.mixHtp({c.h * factor, c.s * factor, c.v * factor});
    }

    // color is actually stored as RGB, so H is R, S is G and V is B:
    matrix.at(0, 1) = HSV(color.r, color.g, color.b);
    matrix.at(0, 2).v = white;
    return matrix;
}

HsvMatrix RgbWLightBlock::getBenchData() const {
    HsvMatrix matrix;
    matrix.rescale(1, 3);
    matrix.at(0, 0).v = m_benchEffects;
    // color is actually stored as RGB, so H is R, S is G and V is B:
    matrix.at(0, 1) = HSV(m_benchColor.getValue().r, m_benchColor.getValue().g, m_benchColor.getValue().b);
    matrix.at(0, 2).v = m_benchWhite;
    return matrix;
}
