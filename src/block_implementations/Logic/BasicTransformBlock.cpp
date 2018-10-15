#include "BasicTransformBlock.h"

#include "core/Nodes.h"


BasicTransformBlock::BasicTransformBlock(MainController* controller, QString uid)
    : InOutBlock(controller, uid)
    , m_rotate(this, "rotate", 0, 0, 270)
    , m_mirrorV(this, "mirrorV", false)
    , m_mirrorH(this, "mirrorH", false)
{
    updateRequestedSize();
    connect(m_outputNode, SIGNAL(requestedSizeChanged()), this, SLOT(updateRequestedSize()));

    connect(m_inputNode, SIGNAL(dataChanged()), this, SLOT(update()));

    connect(&m_rotate, SIGNAL(valueChanged()), this, SLOT(update()));
    connect(&m_mirrorV, SIGNAL(valueChanged()), this, SLOT(update()));
    connect(&m_mirrorH, SIGNAL(valueChanged()), this, SLOT(update()));

    connect(&m_rotate, SIGNAL(valueChanged()), this, SLOT(updateRequestedSize()));
}

void BasicTransformBlock::update() {
    const ColorMatrix& input = m_inputNode->constData();
    auto hsv = HsvDataModifier(m_outputNode);

    if (m_rotate == 45) {
        int diagSize = hsv.width + hsv.height + 1;
        if (input.width() < diagSize || input.height() < diagSize) {
            qWarning() << "BasicTransform: Input too small.";
            return;
        }
        int k = hsv.height - 1;
        for (int x = 0; x < hsv.width; ++x) {
            for (int y = 0; y < hsv.height; ++y) {
                HSV val = input.getHsvAt(k - y + x, x + y);
                hsv.set(x, y, val);
            }
        }
    } else if (m_rotate == 90) {
        if (input.width() < hsv.height || input.height() < hsv.width) {
            qWarning() << "BasicTransform: Input too small.";
            return;
        }
        for (int x = 0; x < hsv.width; ++x) {
            for (int y = 0; y < hsv.height; ++y) {
                HSV val = input.getHsvAt(y, x);
                hsv.set(x, y, val);
            }
        }
    } else if (m_rotate == 180) {
        if (input.width() < hsv.width || input.height() < hsv.height) {
            qWarning() << "BasicTransform: Input too small.";
            return;
        }
        for (int x = 0; x < hsv.width; ++x) {
            for (int y = 0; y < hsv.height; ++y) {
                HSV val = input.getHsvAt(hsv.width - x - 1, hsv.height - y - 1);
                hsv.set(x, y, val);
            }
        }
    } else if (m_rotate == 270) {
        if (input.width() < hsv.height || input.height() < hsv.width) {
            qWarning() << "BasicTransform: Input too small.";
            return;
        }
        for (int x = 0; x < hsv.width; ++x) {
            for (int y = 0; y < hsv.height; ++y) {
                HSV val = input.getHsvAt(hsv.height - y -1, x);
                hsv.set(x, y, val);
            }
        }
    } else {  // no rotation:
        if (input.width() < hsv.width || input.height() < hsv.height) {
            qWarning() << "BasicTransform: Input too small.";
            return;
        }
        for (int x = 0; x < hsv.width; ++x) {
            for (int y = 0; y < hsv.height; ++y) {
                HSV val = input.getHsvAt(x, y);
                hsv.set(x, y, val);
            }
        }
    }

    if (m_mirrorV) {
        for (int x = 0; x < hsv.width; ++x) {
            for (int oy = 0; oy < std::floor(hsv.height / 2); ++oy) {
                HSV tmp = hsv.get(x, oy);
                hsv.set(x, oy, hsv.get(x, hsv.height - oy - 1));
                hsv.set(x, hsv.height - oy - 1, tmp);
            }
        }
    }

    if (m_mirrorH) {
        for (int ox = 0; ox < std::floor(hsv.width / 2); ++ox) {
            for (int y = 0; y < hsv.height; ++y) {
                HSV tmp = hsv.get(ox, y);
                hsv.set(ox, y, hsv.get(hsv.width - ox - 1, y));
                hsv.set(hsv.width - ox - 1, y, tmp);
            }
        }
    }


}

void BasicTransformBlock::updateRequestedSize() {
    if (m_rotate == 90 || m_rotate == 270) {
        Size s = m_outputNode->getRequestedSize();
        m_inputNode->setRequestedSize(Size(s.height, s.width));
    } else if( m_rotate == 45) {
        Size s = m_outputNode->getRequestedSize();
        int diagSize = s.width + s.height + 1;
        m_inputNode->setRequestedSize(Size(diagSize, diagSize));
    } else {
        m_inputNode->setRequestedSize(m_outputNode->getRequestedSize());
    }
}
