#include "ColorizeBlock.h"

#include "core/Nodes.h"


ColorizeBlock::ColorizeBlock(MainController* controller, QString uid)
    : InOutBlock(controller, uid)
    , m_stepsNode(nullptr)
    , m_stepCount(this, "stepCount", 2, 2, 999)
    , m_smooth(this, "smooth", true)
    , m_horizontal(this, "horizontal", true)
{
    m_stepsNode = createInputNode("stepsNode");
    m_outputNode->addNodeSharingRequestedSize(m_inputNode);
    connect(&m_stepCount, &IntegerAttribute::valueChanged, this, &ColorizeBlock::updateRequestedSize);
    connect(&m_smooth, &BoolAttribute::valueChanged, this, &ColorizeBlock::updateOutput);
    connect(&m_horizontal, &BoolAttribute::valueChanged, this, &ColorizeBlock::updateOutput);
    connect(m_stepsNode, &NodeBase::dataChanged, this, &ColorizeBlock::updateOutput);
    connect(m_inputNode, &NodeBase::dataChanged, this, &ColorizeBlock::updateOutput);
    updateRequestedSize();
}

void ColorizeBlock::updateOutput() {
    auto out = RgbDataModifier(m_outputNode);
    const ColorMatrix& input = m_inputNode->constData();
    const ColorMatrix& steps = m_stepsNode->constData();
    int stepCount = steps.width();
    if (input.width() < out.width || input.height() < out.height) {
        qWarning() << "ColorizeBlock: Input too small.";
        return;
    }
    if (stepCount < 2) {
        qWarning() << "ColorizeBlock: Too few steps.";
        return;
    }

    if (m_horizontal) {

        if (stepCount >= out.width) {
            // there are equal or more steps than output pixels
            // -> apply colors using a very simple approach:
            for (int x = 0; x < out.width; ++x) {
                for (int y = 0; y < out.height; ++y) {
                    RGB mix = steps.getRgbAt(x % stepCount, 0);
                    mix = mix * input.getHsvAt(x, y).v;
                    out.set(x, y, mix);
                }
            }
        } else {
            // more pixels than steps -> interpolate between steps:
            double div = (out.width - 1) / double(stepCount - 1);
            bool smooth = m_smooth;
            for (int x = 0; x < out.width; ++x) {
                for (int y = 0; y < out.height; ++y) {
                    int l = int(x / div);
                    int r = qMin(l + 1, stepCount - 1);
                    double ratio = fmod(x / div, 1.0);
                    RGB mix;
                    if (smooth) {
                        mix = steps.getRgbAt(l, 0) * (1 - ratio);
                        mix = mix + (steps.getRgbAt(r, 0) * ratio);
                    } else {
                        mix = steps.getRgbAt( (ratio <= 0.5) ? l : r , 0);
                    }
                    double colMax = qMax(mix.r, qMax(mix.g, mix.b));
                    if (colMax > 0) {
                        mix = mix * (input.getHsvAt(x, y).v / colMax);
                    }
                    out.set(x, y, mix);
                }
            }
        }

    } else {  // vertical

        if (stepCount >= out.height) {
            // there are equal or more steps than output pixels
            // -> apply colors using a very simple approach:
            for (int x = 0; x < out.width; ++x) {
                for (int y = 0; y < out.height; ++y) {
                    RGB mix = steps.getRgbAt(y % stepCount, 0);
                    mix = mix * input.getHsvAt(x, y).v;
                    out.set(x, y, mix);
                }
            }
        } else {
            // more pixels than steps -> interpolate between steps:
            double div = (out.height - 1) / double(stepCount - 1);
            bool smooth = m_smooth;
            for (int x = 0; x < out.width; ++x) {
                for (int y = 0; y < out.height; ++y) {
                    int l = int(y / div);
                    int r = qMin(l + 1, stepCount - 1);
                    double ratio = fmod(y / div, 1.0);
                    RGB mix;
                    if (smooth) {
                        mix = steps.getRgbAt(l, 0) * (1 - ratio);
                        mix = mix + (steps.getRgbAt(r, 0) * ratio);
                    } else {
                        mix = steps.getRgbAt( (ratio <= 0.5) ? l : r , 0);
                    }
                    double colMax = qMax(mix.r, qMax(mix.g, mix.b));
                    if (colMax > 0) {
                        mix = mix * (input.getHsvAt(x, y).v / colMax);
                    }
                    out.set(x, y, mix);
                }
            }
        }

    }
}

void ColorizeBlock::updateRequestedSize() {
    m_stepsNode->setRequestedSize(Size(m_stepCount, 1));
}

