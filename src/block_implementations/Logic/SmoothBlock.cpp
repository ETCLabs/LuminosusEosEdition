#include "SmoothBlock.h"

#include "core/MainController.h"
#include "core/Nodes.h"


SmoothBlock::SmoothBlock(MainController* controller, QString uid)
    : InOutBlock(controller, uid)
    , m_heatUpTime(this, "heatUpTime", 0.1)
    , m_coolDownTime(this, "coolDownTime", 0.1)
    , m_lastValues()
{
    m_outputNode->addNodeSharingRequestedSize(m_inputNode);
    connect(m_outputNode, SIGNAL(requestedSizeChanged()), this, SLOT(updateRequestedSize()));
    connect(m_controller->engine(), SIGNAL(updateBlocks(double)), this, SLOT(eachFrame()));
    updateRequestedSize();
}

void SmoothBlock::eachFrame() {
    auto input = m_inputNode->constData();
    HsvDataModifier out(m_outputNode);
    const double minResolution = 1.0 / (256 * 256);

    for (int x=0; x < out.width; ++x) {
        for (int y=0; y < out.height; ++y) {

            const HSV& col = input.getHsvAt(x, y);
            const double value = col.v;
            double lastValue = m_lastValues.at(x, y).v;
            if (value == lastValue) continue;

            const double diff = value - lastValue;
            if (qAbs(diff) < minResolution) {
                // difference is even below 16bit resolution
                lastValue = value;
            } else if (diff > 0) {
                // value is going up
                if (m_heatUpTime <= 0) {
                    lastValue = value;
                } else {
                    const double change = qMax(diff / (1 + 40.0 * qPow(m_heatUpTime, 1.3)), minResolution);
                    lastValue = qMin(lastValue + change, value);
                }
            } else {
                // value is going down
                if (m_coolDownTime <= 0) {
                    lastValue = value;
                } else {
                    const double change = qMax(-diff / (1 + 40.0 * qPow(m_coolDownTime, 1.3)), minResolution);
                    lastValue = qMax(lastValue - change, value);
                }
            }
            m_lastValues.at(x, y).v = lastValue;
            out.set(x, y, col.h, col.s, lastValue);

        }
    }
}

void SmoothBlock::updateRequestedSize() {
    m_lastValues.rescale(m_outputNode->getRequestedSize());
}
