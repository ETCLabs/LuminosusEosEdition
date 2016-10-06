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

#include "Nodes.h"
#include "utils.h"


ColorwheelBlock::ColorwheelBlock(MainController* controller, QString uid)
    : InOutBlock(controller, uid, info().qmlFile)
    , m_hue(0)
    , m_sat(1)
{
    connect(m_inputNode, SIGNAL(dataChanged()), this, SLOT(updateOutput()));
}

QJsonObject ColorwheelBlock::getState() const {
    QJsonObject state;
    state["hue"] = getHue();
    state["sat"] = getSat();
    return state;
}

void ColorwheelBlock::setState(const QJsonObject &state) {
    setHue(state["hue"].toDouble());
    setSat(state["sat"].toDouble());
}

void ColorwheelBlock::updateOutput() {
    double value = 1;
    if (m_inputNode->isConnected()) {
        value = m_inputNode->getValue();
    }
    m_outputNode->getDataToModify().setHsv(m_hue, m_sat, value);
    m_outputNode->dataWasModifiedByBlock();
}

void ColorwheelBlock::setHue(double value) {
    if (value == m_hue) return;
    m_hue = limit(0, value, 1);
    emit hueChanged();
    updateOutput();
}

void ColorwheelBlock::setSat(double value) {
    if (value == m_sat) return;
    m_sat = limit(0, value, 1);
    emit satChanged();
    updateOutput();
}
