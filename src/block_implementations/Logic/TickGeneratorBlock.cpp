#include "TickGeneratorBlock.h"

#include "core/MainController.h"
#include "core/Nodes.h"


TickGeneratorBlock::TickGeneratorBlock(MainController* controller, QString uid)
    : InOutBlock(controller, uid)
    , m_stepTime(1.0)
    , m_pos(0.0)
{
    connect(m_inputNode, SIGNAL(dataChanged()), this, SLOT(onInputChanged()));
    connect(m_controller->engine(), SIGNAL(updateBlocks(double)), this, SLOT(eachFrame(double)));
}

void TickGeneratorBlock::getAdditionalState(QJsonObject& state) const {
    state["stepTime"] = getStepTime();
}

void TickGeneratorBlock::setAdditionalState(const QJsonObject& state) {
    setStepTime(state["stepTime"].toDouble());
}

void TickGeneratorBlock::onInputChanged() {
    if (!m_inputNode->constData().absoluteMaximumIsProvided()) return;
    double absoluteValue = m_inputNode->getAbsoluteValue();
    setStepTime(absoluteValue);
}

void TickGeneratorBlock::sync() {
    m_pos = 1;
}

void TickGeneratorBlock::eachFrame(double timeSinceLastFrame) {
    // advance position value:
    double stepTime = m_stepTime;
    if (m_inputNode->isConnected() && !m_inputNode->constData().absoluteMaximumIsProvided()) {
        // there is a non-absolute Block connected:
        stepTime *= m_inputNode->getValue();
    }
    if (stepTime == 0) return;
    double progress = timeSinceLastFrame / stepTime;
    if ((m_pos + progress) > 1) {
        m_framesLeft = 3;
        m_outputNode->setValue(1.0);
    } else if (m_framesLeft == 1) {
        m_outputNode->setValue(0.0);
        m_framesLeft = 0;
    } else if (m_framesLeft) {
        --m_framesLeft;
    }
    m_pos = fmod(m_pos + progress, 1.0);
}
