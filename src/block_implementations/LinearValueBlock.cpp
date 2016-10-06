#include "LinearValueBlock.h"

#include "MainController.h"
#include "Nodes.h"


LinearValueBlock::LinearValueBlock(MainController* controller, QString uid)
    : InOutBlock(controller, uid, info().qmlFile)
    , m_stepTime(1.0)
    , m_pos(0.0)
{
    connect(m_inputNode, SIGNAL(dataChanged()), this, SLOT(onInputChanged()));
    connect(m_controller->engine(), SIGNAL(updateBlocks(double)), this, SLOT(eachFrame(double)));
}

QJsonObject LinearValueBlock::getState() const {
    QJsonObject state;
    state["stepTime"] = getStepTime();
    return state;
}

void LinearValueBlock::setState(const QJsonObject& state) {
    setStepTime(state["stepTime"].toDouble());
}

void LinearValueBlock::onInputChanged() {
    if (!m_inputNode->getDataToRead().absoluteMaximumIsProvided()) return;
    double absoluteValue = m_inputNode->getAbsoluteValue();
    setStepTime(absoluteValue);
}

void LinearValueBlock::sync() {
    m_pos = 1;
}

void LinearValueBlock::eachFrame(double timeSinceLastFrame) {
    // advance position value:
    double stepTime = m_stepTime;
    if (m_inputNode->isConnected() && !m_inputNode->getDataToRead().absoluteMaximumIsProvided()) {
        // there is a non-absolute Block connected:
        stepTime *= m_inputNode->getValue();
    }
    if (stepTime == 0) return;
    double progress = timeSinceLastFrame / stepTime;
    m_pos = fmod(m_pos + progress, 1.0);
    m_outputNode->setValue(m_pos);
}
