#include "CrossfadeBlock.h"

#include "core/Nodes.h"


CrossfadeBlock::CrossfadeBlock(MainController* controller, QString uid)
    : InOutBlock(controller, uid)
    , m_ratio(this, "ratio", 0.0)
{
    m_inputNode2 = createInputNode("inputNode2");
    m_ratioNode = createInputNode("ratioNode");

    m_outputNode->addNodeSharingRequestedSize(m_inputNode);
    m_outputNode->addNodeSharingRequestedSize(m_inputNode2);

    connect(m_inputNode, SIGNAL(dataChanged()), this, SLOT(update()));
    connect(m_inputNode2, SIGNAL(dataChanged()), this, SLOT(update()));

    connect(&m_ratio, SIGNAL(valueChanged()), this, SLOT(update()));

    connect(m_ratioNode, &NodeBase::dataChanged, [this](){ m_ratio.setValue(m_ratioNode->getValue()); });
}

void CrossfadeBlock::update() {
    if (m_inputNode->constData().absoluteMaximumIsProvided()
            || m_inputNode2->constData().absoluteMaximumIsProvided()) {
        updateAbsolute();
    } else {
        updateRgb();
    }
}

void CrossfadeBlock::updateRgb() {
    const ColorMatrix& input1 = m_inputNode->constData();
    const ColorMatrix& input2 = m_inputNode2->constData();
    auto out = RgbDataModifier(m_outputNode);
    if (input1.width() < out.width || input1.height() < out.height
            || input2.width() < out.width || input2.height() < out.height) {
        qWarning() << "Crossfade: Input too small.";
        return;
    }

    double ratio = m_ratio;

    for (int x = 0; x < out.width; ++x) {
        for (int y = 0; y < out.height; ++y) {
            RGB mix = input1.getRgbAt(x, y) * (1 - ratio);
            mix = mix + input2.getRgbAt(x, y) * ratio;
            out.set(x, y, mix);
        }
    }
}

void CrossfadeBlock::updateAbsolute() {
    const double v1 = m_inputNode->getAbsoluteValue();
    const double v2 = m_inputNode2->getAbsoluteValue();
    double ratio = m_ratio;
    double result = v1 * (1-ratio) + v2 * ratio;
    m_outputNode->setAbsoluteValue(result);
}
