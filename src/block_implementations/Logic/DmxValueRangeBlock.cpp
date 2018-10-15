#include "DmxValueRangeBlock.h"

#include "core/MainController.h"
#include "core/Nodes.h"


DmxValueRangeBlock::DmxValueRangeBlock(MainController* controller, QString uid)
    : InOutBlock(controller, uid)
    , m_switch(this, "switch", true)
    , m_low(this, "low", 0, 0, 255)
    , m_high(this, "high", 255, 0, 255)
{
    m_outputNode->addNodeSharingRequestedSize(m_inputNode);
    connect(m_inputNode, SIGNAL(dataChanged()), this, SLOT(updateOutput()));
    connect(&m_switch, &BoolAttribute::valueChanged, this, &DmxValueRangeBlock::updateOutput);
    connect(&m_low, &IntegerAttribute::valueChanged, this, &DmxValueRangeBlock::updateOutput);
    connect(&m_high, &IntegerAttribute::valueChanged, this, &DmxValueRangeBlock::updateOutput);
}

void DmxValueRangeBlock::updateOutput() {
    int dmxValue = qRound(m_inputNode->getValue() * 255);
    int diff = m_high - m_low;
    bool inRange = dmxValue >= qMin(m_low, m_high) && dmxValue <= qMax(m_high, m_low);

    double v = inRange ? (!m_switch && diff != 0 ? (double(dmxValue - m_low) / diff) : 1.0) : 0.0;
    m_outputNode->setValue(v);
}
