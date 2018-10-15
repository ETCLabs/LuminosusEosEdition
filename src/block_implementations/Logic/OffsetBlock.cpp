#include "OffsetBlock.h"

#include "core/Nodes.h"


OffsetBlock::OffsetBlock(MainController* controller, QString uid)
    : InOutBlock(controller, uid)
    , m_offsetX(this, "offsetX", 0.0)
    , m_offsetY(this, "offsetY", 0.0)
{
    m_outputNode->addNodeSharingRequestedSize(m_inputNode);
    connect(m_inputNode, SIGNAL(dataChanged()), this, SLOT(update()));

    connect(&m_offsetX, SIGNAL(valueChanged()), this, SLOT(update()));
    connect(&m_offsetY, SIGNAL(valueChanged()), this, SLOT(update()));
}

void OffsetBlock::update() {
    const ColorMatrix& input = m_inputNode->constData();
    auto out = RgbDataModifier(m_outputNode);
    if (input.width() < out.width || input.height() < out.height) {
        qWarning() << "Offset: Input too small.";
        return;
    }

    // invert offset to make direction more intuitiv:
    double offX = 1.0 - m_offsetX;
    double offY = 1.0 - m_offsetY;

    // calculate offset from full pixel:
    double rxi = fmod(offX * out.width, 1.0);
    double rx = 1.0 - rxi;
    double ryi = fmod(offY * out.height, 1.0);
    double ry = 1.0 - ryi;

    for (int x = 0; x < out.width; ++x) {
        for (int y = 0; y < out.height; ++y) {
            int x1 = int(x + offX * out.width) % out.width;
            int x2 = (int(x + offX * out.width) + 1) % out.width;
            int y1 = int(y + offY * out.height) % out.height;
            int y2 = (int(y + offY * out.height) + 1) % out.height;
            RGB mix = input.getRgbAt(x1, y1) * (rx * ry);
            mix = mix + input.getRgbAt(x2, y2) * (rxi * ryi);
            mix = mix + input.getRgbAt(x2, y1) * (rxi * ry);
            mix = mix + input.getRgbAt(x1, y2) * (rx * ryi);
            out.set(x, y, mix);
        }
    }
}
