#include "VerticalPatchBlock.h"

#include "core/Nodes.h"


VerticalPatchBlock::VerticalPatchBlock(MainController* controller, QString uid)
    : BlockBase(controller, uid)
{
    // prepare nodes:
    m_inputNode = createInputNode("inputNode");
    m_outputNode1 = createOutputNode("outputNode1");
    m_outputNode2 = createOutputNode("outputNode2");

    // set initial requested size:
    updateRequestedSize();

    // connect signals and slots:
    connect(m_outputNode1, SIGNAL(requestedSizeChanged()), this, SLOT(updateRequestedSize()));
    connect(m_outputNode2, SIGNAL(requestedSizeChanged()), this, SLOT(updateRequestedSize()));

    connect(m_inputNode, SIGNAL(dataChanged()), this, SLOT(updateOutput()));
}

void VerticalPatchBlock::updateOutput() {
    const ColorMatrix& input = m_inputNode->constData();

    auto out1 = RgbDataModifier(m_outputNode1);
    auto out2 = RgbDataModifier(m_outputNode2);
    if (input.width() < qMax(out1.width, out2.width) || input.height() < out1.height + out2.height) {
        qWarning() << "V-Patch data too small " << input.width() << input.height();
        return;
    }

    for (int x = 0; x < out1.width; ++x) {
        for (int y = 0; y < out1.height; ++y) {
            auto color = input.getRgbAt(x, y);
            out1.set(x, y, color.r, color.g, color.b);
        }
    }

    for (int x = 0; x < out2.width; ++x) {
        for (int y = 0; y < out2.height; ++y) {
            auto color = input.getRgbAt(x, y + out1.height);
            out2.set(x, y, color.r, color.g, color.b);
        }
    }
}

void VerticalPatchBlock::updateRequestedSize() {
    Size requestedSize = m_outputNode1->getRequestedSize();
    Size requestedSize2 = m_outputNode2->getRequestedSize();
    requestedSize.width = qMax(requestedSize.width, requestedSize2.width);
    requestedSize.height += requestedSize2.height;
    Size oldSize = m_inputNode->getRequestedSize();
    if (oldSize.width != requestedSize.width || oldSize.height != requestedSize.height) {
        m_inputNode->setRequestedSize(requestedSize);
    }
    // at least one output changed (even if not total size) -> update it:
    updateOutput();
}
