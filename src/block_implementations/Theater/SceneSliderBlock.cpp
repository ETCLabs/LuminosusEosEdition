#include "SceneSliderBlock.h"

#include "core/MainController.h"
#include "core/Nodes.h"


SceneSliderBlock::SceneSliderBlock(MainController* controller, QString uid)
    : BlockBase(controller, uid)
    , m_outputNode(nullptr)
    , m_benchValue(this, "benchValue")
    , m_resultValue(this, "resultValue")
{
    m_heightIsResizable = true;

    m_isSceneBlock = true;
    m_outputNode = createOutputNode("outputNode");

    connect(&m_benchValue, SIGNAL(valueChanged()), this, SLOT(update()));
}

void SceneSliderBlock::update() {
    double val = m_benchValue;
    for (const SceneData& data: m_sceneValues) {
        double factor = data.factor;
        val = qMax(val, data.values.at(0, 0).v * factor);
    }
    m_resultValue = val;

    m_outputNode->setValue(m_resultValue);
}

void SceneSliderBlock::addToBench(const SceneData& data) {
    double factor = data.factor;
    double val = data.values.at(0, 0).v;
    m_benchValue = qMax(m_benchValue.getValue(), val * factor);
}

void SceneSliderBlock::clearBench() {
    m_benchValue = 0.0;
}

HsvMatrix SceneSliderBlock::getMixData() const {
    HsvMatrix matrix;
    matrix.rescale(1, 1);
    matrix.at(0, 0).v = m_resultValue;
    return matrix;
}

HsvMatrix SceneSliderBlock::getBenchData() const {
    HsvMatrix matrix;
    matrix.rescale(1, 1);
    matrix.at(0, 0).v = m_benchValue;
    return matrix;
}
