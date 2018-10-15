// Copyright (c) 2016 Electronic Theatre Controls, Inc., http://www.etcconnect.com
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "ThresholdBlock.h"

#include "core/Nodes.h"


ThresholdBlock::ThresholdBlock(MainController* controller, QString uid)
	: InOutBlock(controller, uid)
	, m_threshold(0.5)
{
	m_outputNodeBelow = createOutputNode("outputNodeBelow");
	connect(m_inputNode, SIGNAL(dataChanged()), this, SLOT(updateOutput()));
	connect(this, SIGNAL(thresholdChanged()), this, SLOT(updateOutput()));
    connect(m_outputNode, SIGNAL(requestedSizeChanged()), this, SLOT(updateRequestedSize()));
    connect(m_outputNodeBelow, SIGNAL(requestedSizeChanged()), this, SLOT(updateRequestedSize()));
}

void ThresholdBlock::getAdditionalState(QJsonObject& state) const {
    state["threshold"] = getThreshold();
}

void ThresholdBlock::setAdditionalState(const QJsonObject &state) {
	setThreshold(state["threshold"].toDouble());
}

void ThresholdBlock::updateOutput() {
    double value = m_inputNode->getValue();
    setInputValue(value);

    if (m_outputNode->getRequestedSize().pixels() == 1
            && m_outputNodeBelow->getRequestedSize().pixels() == 1) {
        if (value > m_threshold) {
            m_outputNode->setValue(1.0);
            m_outputNodeBelow->setValue(0.0);
        } else {
            m_outputNode->setValue(0.0);
            m_outputNodeBelow->setValue(1.0);
        }
    } else {
        if (m_outputNode->isConnected()) {
            auto out = HsvDataModifier(m_outputNode);
            const ColorMatrix& input = m_inputNode->constData();
            if (input.width() < out.width || input.height() < out.height) {
                qWarning() << "ThresholdBlock: Input too small.";
                return;
            }

            for (int x = 0; x < out.width; ++x) {
                for (int y = 0; y < out.height; ++y) {
                    HSV col = input.getHsvAt(x, y);
                    if (col.v > m_threshold) {
                        out.set(x, y, col.h, col.s, 1.0);
                    } else {
                        out.set(x, y, col.h, col.s, 0.0);
                    }
                }
            }
        }
        if (m_outputNodeBelow->isConnected()) {
            auto out = HsvDataModifier(m_outputNodeBelow);
            const ColorMatrix& input = m_inputNode->constData();
            if (input.width() < out.width || input.height() < out.height) {
                qWarning() << "ThresholdBlock: Input too small.";
                return;
            }

            for (int x = 0; x < out.width; ++x) {
                for (int y = 0; y < out.height; ++y) {
                    HSV col = input.getHsvAt(x, y);
                    if (col.v > m_threshold) {
                        out.set(x, y, col.h, col.s, 0.0);
                    } else {
                        out.set(x, y, col.h, col.s, 1.0);
                    }
                }
            }
        }
    }
}

void ThresholdBlock::updateRequestedSize() {
    Size a = m_outputNode->getRequestedSize();
    Size b = m_outputNodeBelow->getRequestedSize();
    m_inputNode->setRequestedSize(Size(qMax(a.width, b.width), qMax(a.height, b.height)));
}
