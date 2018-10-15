#include "OneDimensionalPattern.h"

#include "core/Nodes.h"


OneDimensionalPattern::OneDimensionalPattern(MainController* controller, QString uid)
    : InOutBlock(controller, uid)
    , m_pattern(this, "pattern", 0, 0, 1)
{
    connect(m_inputNode, SIGNAL(dataChanged()), this, SLOT(updateOutput()));
    connect(m_outputNode, SIGNAL(requestedSizeChanged()), this, SLOT(updateRequestedSize()));
    connect(&m_pattern, SIGNAL(valueChanged()), this, SLOT(updateOutput()));
}

void OneDimensionalPattern::updateOutput() {
    if (m_pattern == 0) {
        bar();
    } else {
        dot();
    }
}

void OneDimensionalPattern::updateRequestedSize() {
    int requestedHeight = m_outputNode->getRequestedSize().height;
    m_inputNode->setRequestedSize(Size(requestedHeight, 1));
}

void OneDimensionalPattern::bar() {
    const ColorMatrix& input = m_inputNode->constData();
    auto hsv = HsvDataModifier(m_outputNode);
    if (input.width() < hsv.height) {
        qWarning() << "Pattern: Input too small.";
        return;
    }

    for (int x = 0; x < hsv.width; ++x) {
        for (int y = 0; y < hsv.height; ++y) {
            double barEnd = input.getHsvAt(y, 0).v * hsv.width;
            double val = limit(0.0, barEnd - x, 1.0);
            hsv.set(x, y, 0, 0, val);
        }
    }
}

void OneDimensionalPattern::dot() {
    const ColorMatrix& input = m_inputNode->constData();
    auto hsv = HsvDataModifier(m_outputNode);
    if (input.width() < hsv.height) {
        qWarning() << "Pattern: Input too small.";
        return;
    }

    for (int x = 0; x < hsv.width; ++x) {
        for (int y = 0; y < hsv.height; ++y) {
            double barEnd = input.getHsvAt(y, 0).v * hsv.width;
            double val = (x == int(barEnd)) ? 1.0 : 0.0;
            hsv.set(x, y, 0, 0, val);
        }
    }
}
