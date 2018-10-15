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

#include "SliderBlock.h"

#include "core/MainController.h"
#include "core/Nodes.h"
#include "utils.h"


SliderBlock::SliderBlock(MainController* controller, QString uid)
    : OneOutputBlock(controller, uid)
{
    m_heightIsResizable = true;
}

void SliderBlock::getAdditionalState(QJsonObject& state) const {
    state["value"] = getValue();
}

void SliderBlock::setAdditionalState(const QJsonObject &state) {
    setValue(state["value"].toDouble());
}

void SliderBlock::onControllerPressed() {
	m_valueBeforeFlash = getValue();
    if (getGuiItemChild("slider")) {
        getGuiItemChild("slider")->setProperty("value", m_valueBeforeFlash < 0.99 ? 1 : 0);
    } else {
        qWarning() << "FIXME: SliderBlock doesn't work without GUI!";
    }
	m_controller->powermate()->setBrightness(1.);
}

void SliderBlock::onControllerReleased(double) {
    if (getGuiItemChild("slider")) {
        getGuiItemChild("slider")->setProperty("value", m_valueBeforeFlash);
    } else {
        qWarning() << "FIXME: SliderBlock doesn't work without GUI!";
    }
	m_controller->powermate()->setBrightness(m_valueBeforeFlash);
}

void SliderBlock::onControllerRotated(double relativeAmount, double, bool) {
    double newValue = getValue() + relativeAmount;
    if (getGuiItemChild("slider")) {
        getGuiItemChild("slider")->setProperty("value", limit(0., newValue, 1.));
    } else {
        qWarning() << "FIXME: SliderBlock doesn't work without GUI!";
    }
	m_controller->powermate()->setBrightness(newValue);
}
