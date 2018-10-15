#include "HsvBlock.h"

HsvBlock::HsvBlock(MainController *controller, QString uid)
    : BlockBase(controller, uid)
    , m_outputNode(nullptr)
    , m_inputX(nullptr)
    , m_inputY(nullptr)
    , m_inputZ(nullptr)
{
    // prepare nodes:
    m_outputNode = createOutputNode("outputNode");
    m_inputX = createInputNode("inputX");
    m_inputY = createInputNode("inputY");
    m_inputZ = createInputNode("inputZ");

    m_outputNode->addNodeSharingRequestedSize(m_inputX);
    m_outputNode->addNodeSharingRequestedSize(m_inputY);
    m_outputNode->addNodeSharingRequestedSize(m_inputZ);

    //connect signals and slots:
    connect(m_inputX, SIGNAL(dataChanged()), this, SLOT(updateOutput()));
    connect(m_inputY, SIGNAL(dataChanged()), this, SLOT(updateOutput()));
    connect(m_inputZ, SIGNAL(dataChanged()), this, SLOT(updateOutput()));
}

void HsvBlock::updateOutput() {
    if (m_outputNode->getRequestedSize().pixels() == 1) {
        const double hue = m_inputX->isConnected() ? m_inputX->getValue() : 0.0;
        const double sat = m_inputY->isConnected() ? m_inputY->getValue() : 1.0;
        const double val = m_inputZ->isConnected() ? m_inputZ->getValue() : 1.0;

        m_outputNode->setHsv(hue, sat, val);
    } else {
        auto out = HsvDataModifier(m_outputNode);
        ColorMatrix& inputX = m_inputX->data();
        ColorMatrix& inputY = m_inputY->data();
        ColorMatrix& inputZ = m_inputZ->data();
        if (inputX.width() < out.width || inputX.height() < out.height
                || inputY.width() < out.width || inputY.height() < out.height
                || inputZ.width() < out.width || inputZ.height() < out.height) {
            qWarning() << "HsvBlock: Input too small.";
            return;
        }

        if (!m_inputX->isConnected()) inputX.setHsv(0, 0, 0);
        if (!m_inputY->isConnected()) inputY.setHsv(0, 0, 1);
        if (!m_inputZ->isConnected()) inputZ.setHsv(0, 0, 1);

        for (int x = 0; x < out.width; ++x) {
            for (int y = 0; y < out.height; ++y) {
                out.set(x, y, inputX.getHsvAt(x, y).v, inputY.getHsvAt(x, y).v, inputZ.getHsvAt(x, y).v);
            }
        }
    }
}
