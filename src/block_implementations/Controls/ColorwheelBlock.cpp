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

#include "ColorwheelBlock.h"

#include "core/Nodes.h"
#include "utils.h"


ColorwheelBlock::ColorwheelBlock(MainController* controller, QString uid)
    : InOutBlock(controller, uid)
    , m_hue(this, "hue", 0.0)
    , m_sat(this, "sat", 1.0)
    , m_val(this, "val", 1.0)
{
    m_outputNode->addNodeSharingRequestedSize(m_inputNode);
    connect(m_inputNode, SIGNAL(dataChanged()), this, SLOT(updateOutput()));
    connect(&m_hue, &DoubleAttribute::valueChanged, this, &ColorwheelBlock::updateOutput);
    connect(&m_sat, &DoubleAttribute::valueChanged, this, &ColorwheelBlock::updateOutput);
    connect(&m_val, &DoubleAttribute::valueChanged, this, &ColorwheelBlock::updateOutput);
}

void ColorwheelBlock::updateOutput() {
    if (m_inputNode->isConnected()) {
        if (m_outputNode->getRequestedSize().pixels() == 1) {
            double value = m_inputNode->getValue();
            m_outputNode->data().setHsv(m_hue, m_sat, value);
            m_outputNode->dataWasModifiedByBlock();
        } else {
            auto out = HsvDataModifier(m_outputNode);
            const ColorMatrix& input = m_inputNode->constData();
            if (input.width() < out.width || input.height() < out.height) {
                qWarning() << "ColorwheelBlock: Input too small.";
                return;
            }

            for (int x = 0; x < out.width; ++x) {
                for (int y = 0; y < out.height; ++y) {
                    double val = input.getHsvAt(x, y).v;
                    out.set(x, y, m_hue, m_sat, val);
                }
            }
        }
    } else {
        m_outputNode->data().setHsv(m_hue, m_sat, 1.0);
        m_outputNode->dataWasModifiedByBlock();
    }
}
