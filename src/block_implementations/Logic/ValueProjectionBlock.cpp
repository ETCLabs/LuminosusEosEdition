#include "ValueProjectionBlock.h"

#include "core/MainController.h"
#include "core/Nodes.h"


ValueProjectionBlock::ValueProjectionBlock(MainController* controller, QString uid)
    : InOutBlock(controller, uid)
    , m_inMin(this, "inMin", 0.0)
    , m_inMax(this, "inMax", 1.0)
    , m_outMin(this, "outMin", 0.0)
    , m_outMax(this, "outMax", 1.0)
{
    m_outputNode->addNodeSharingRequestedSize(m_inputNode);
    connect(m_inputNode, SIGNAL(dataChanged()), this, SLOT(updateOutput()));
    connect(&m_inMin, &DoubleAttribute::valueChanged, this, &ValueProjectionBlock::updateOutput);
    connect(&m_inMax, &DoubleAttribute::valueChanged, this, &ValueProjectionBlock::updateOutput);
    connect(&m_outMin, &DoubleAttribute::valueChanged, this, &ValueProjectionBlock::updateOutput);
    connect(&m_outMax, &DoubleAttribute::valueChanged, this, &ValueProjectionBlock::updateOutput);
}

void ValueProjectionBlock::updateOutput() {
    if (m_inMin == m_inMax || m_outMin == m_outMax) return;

    if (m_outputNode->getRequestedSize().pixels() == 1) {
        double value = m_inputNode->getValue();

        value = (value - m_inMin) / (m_inMax - m_inMin);
        value = limit(0, value, 1);
        value = (value * (m_outMax - m_outMin)) + m_outMin;

        m_outputNode->setValue(value);
    } else {
        auto out = HsvDataModifier(m_outputNode);
        const ColorMatrix& input = m_inputNode->constData();
        if (input.width() < out.width || input.height() < out.height) {
            qWarning() << "ValueProjectionBlock: Input too small.";
            return;
        }

        for (int x = 0; x < out.width; ++x) {
            for (int y = 0; y < out.height; ++y) {
                HSV col = input.getHsvAt(x, y);

                col.v = (col.v - m_inMin) / (m_inMax - m_inMin);
                col.v = limit(0, col.v, 1);
                col.v = (col.v * (m_outMax - m_outMin)) + m_outMin;

                out.set(x, y, col);
            }
        }
    }
}
