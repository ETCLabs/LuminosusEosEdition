#include "MultiplyBlock.h"

#include "core/Nodes.h"


MultiplyBlock::MultiplyBlock(MainController* controller, QString uid)
    : InOutBlock(controller, uid)
{
    m_inputNode2 = createInputNode("inputNode2");

    m_outputNode->addNodeSharingRequestedSize(m_inputNode);
    m_outputNode->addNodeSharingRequestedSize(m_inputNode2);

    connect(m_inputNode, SIGNAL(dataChanged()), this, SLOT(update()));
    connect(m_inputNode2, SIGNAL(dataChanged()), this, SLOT(update()));
}

void MultiplyBlock::update() {
    const ColorMatrix& input1 = m_inputNode->constData();
    const ColorMatrix& input2 = m_inputNode2->constData();
    RgbDataModifier out(m_outputNode);

    if (input1.width() < out.width || input1.height() < out.height
            || input2.width() < out.width || input2.height() < out.height) {
        qWarning() << "Multiply: Input too small.";
        return;
    }

    for (int x = 0; x < out.width; ++x) {
        for (int y = 0; y < out.height; ++y) {
            RGB in1 = input1.getRgbAt(x, y);
            RGB in2 = input2.getRgbAt(x, y);
            in1.r *= in2.r;
            in1.g *= in2.g;
            in1.b *= in2.b;
            out.set(x, y, in1);
        }
    }
}
