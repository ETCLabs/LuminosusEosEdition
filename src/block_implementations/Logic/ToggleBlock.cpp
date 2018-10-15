#include "ToggleBlock.h"

#include "core/Nodes.h"
#include "core/MainController.h"  // for LuminosusConstants


ToggleBlock::ToggleBlock(MainController* controller, QString uid)
    : InOutBlock(controller, uid)
    , m_onNode(nullptr)
    , m_offNode(nullptr)
    , m_value(this, "value")
{
    m_onNode = createInputNode("on");
    m_offNode = createInputNode("off");

    m_inputNode->enableImpulseDetection();
    connect(m_inputNode, SIGNAL(impulseBegin()), this, SLOT(toggle()));
    m_onNode->enableImpulseDetection();
    connect(m_onNode, SIGNAL(impulseBegin()), this, SLOT(on()));
    m_onNode->setActive(true);
    m_offNode->enableImpulseDetection();
    connect(m_offNode, SIGNAL(impulseBegin()), this, SLOT(off()));
    m_offNode->setActive(false);
}

void ToggleBlock::setAdditionalState(const QJsonObject&) {
    m_outputNode->setValue(m_value);
    if (m_value > LuminosusConstants::triggerThreshold) {
        m_onNode->setActive(false);
        m_offNode->setActive(true);
    } else {
        m_onNode->setActive(true);
        m_offNode->setActive(false);
    }
}

void ToggleBlock::toggle() {
    if (m_value > LuminosusConstants::triggerThreshold) {
        m_value = 0.0;
        m_onNode->setActive(true);
        m_offNode->setActive(false);
    } else {
        m_value = 1.0;
        m_onNode->setActive(false);
        m_offNode->setActive(true);
    }
    m_outputNode->setValue(m_value);
}

void ToggleBlock::on() {
    m_value = 1.0;
    m_outputNode->setValue(m_value);
    m_onNode->setActive(false);
    m_offNode->setActive(true);
}

void ToggleBlock::off() {
    m_value = 0.0;
    m_outputNode->setValue(m_value);
    m_onNode->setActive(true);
    m_offNode->setActive(false);
}
