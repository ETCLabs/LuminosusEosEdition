#include "FixtureBlock.h"

#include "core/MainController.h"
#include "core/Nodes.h"

FixtureBlock::FixtureBlock(MainController *controller, QString uid, int footprint)
    : InOutBlock(controller, uid)
    , m_footprint(footprint)
    , m_address(this, "address", 1, 1, 8193 - m_footprint)
    , m_gamma(this, "gamma", 1.0, 0.1, 10.0)
{
    m_isSceneBlock = true;

    // connect signals and slots:
    connectSlots(m_inputNode, m_outputNode);

    connect(&m_address, SIGNAL(valueChanged()), this, SLOT(notifyAboutAddress()));
}

void FixtureBlock::forwardDataToOutput() {
    // pass through data to output node:
    if (m_outputNode->isConnected()) {
        const ColorMatrix& input = m_inputNode->constData();
        auto rgb = RgbDataModifier(m_outputNode);
        if (input.width() < rgb.width + 1 || input.height() < rgb.height) {
            qWarning() << "FixtureBlock forward: data too small";
            return;
        }
        for (int x = 0; x < rgb.width; ++x) {
            for (int y = 0; y < rgb.height; ++y) {
                auto color = input.getRgbAt(x + 1, y);
                rgb.set(x, y, color.r, color.g, color.b);
            }
        }
    }
}

void FixtureBlock::forwardData(NodeBase* inputNode, NodeBase* outputNode) {
    // pass through data to output node:
    if (outputNode->isConnected()) {
        const ColorMatrix& input = inputNode->constData();
        auto rgb = RgbDataModifier(outputNode);
        if (input.width() < rgb.width + 1 || input.height() < rgb.height) {
            qWarning() << "FixtureBlock forward: data too small";
            return;
        }
        for (int x = 0; x < rgb.width; ++x) {
            for (int y = 0; y < rgb.height; ++y) {
                auto color = input.getRgbAt(x + 1, y);
                rgb.set(x, y, color.r, color.g, color.b);
            }
        }
    }
}

void FixtureBlock::onConnectionChanged(NodeBase* inputNode, NodeBase* outputNode) {
    if (outputNode->isConnected()) {
        // new requested size is requested size of previous node + 1 in x-direction
        Size requestedSize = outputNode->getRequestedSize();
        requestedSize.width += 1;
        inputNode->setRequestedSize(requestedSize);
        forwardData(inputNode, outputNode);
    } else {
        // if ouput is not connected requested size is just one pixel:
        inputNode->setRequestedSize(Size(1, 1));
    }
}

void FixtureBlock::connectSlots(NodeBase* inputNode, NodeBase* outputNode) {
    connect(inputNode, &NodeBase::connectionChanged, [inputNode, outputNode](){FixtureBlock::onConnectionChanged(inputNode, outputNode);} );
    connect(outputNode, &NodeBase::connectionChanged, [inputNode, outputNode](){FixtureBlock::onConnectionChanged(inputNode, outputNode);} );
    connect(outputNode, &NodeBase::requestedSizeChanged, [inputNode, outputNode](){FixtureBlock::onConnectionChanged(inputNode, outputNode);} );
}

void FixtureBlock::notifyAboutAddress() {
    m_controller->output()->setNextAddressToUse(m_address + m_footprint);
}
