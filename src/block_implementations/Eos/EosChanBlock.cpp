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

#include "EosChanBlock.h"

#include "core/MainController.h"
#include "core/Nodes.h"

EosChanBlock::EosChanBlock(MainController *controller, QString uid)
    : OneInputBlock(controller, uid)
	, m_chanNumber(1)
    , m_lastValue()
    , m_valueText("")
{
    m_outputNode = createOutputNode("outputNode");

    // connect signals and slots:
    connect(m_inputNode, SIGNAL(connectionChanged()), this, SLOT(onConnectionChanged()));
    connect(m_outputNode, SIGNAL(connectionChanged()), this, SLOT(onConnectionChanged()));
    connect(m_outputNode, SIGNAL(requestedSizeChanged()), this, SLOT(onConnectionChanged()));

	connect(m_inputNode, SIGNAL(dataChanged()), this, SLOT(onValueChanged()));
}

void EosChanBlock::getAdditionalState(QJsonObject& state) const {
    state["chanNumber"] = getChannelNumber();
}

void EosChanBlock::setAdditionalState(const QJsonObject &state) {
	setChannelNumber(state["chanNumber"].toInt());
}

void EosChanBlock::onValueChanged() {
    HSV hsv = m_inputNode->constData().getHsvAt(0, 0);
    if (!qFuzzyCompare(1 + hsv.h, 1 + m_lastValue.h) || !qFuzzyCompare(1 + hsv.s, 1 + m_lastValue.s)) {
        QString message = "/eos/user/0/chan/%1/param/hue/saturation";
        message = message.arg(QString::number(m_chanNumber));
        m_controller->lightingConsole()->sendMessage(message, hsv.h * 360, hsv.s * 100);
    }
    if (hsv.v != m_lastValue.v) {
        QString message = "/eos/user/0/chan/%1";
        message = message.arg(QString::number(m_chanNumber));
        m_controller->lightingConsole()->sendMessage(message, hsv.v * 100);
    }
    m_lastValue = hsv;
    emit lastValueChanged();
    setValueText(hsv.v == 1 ? "FL" : QString::number(qFloor(hsv.v * 100)));

    forwardDataToOutput();
}

void EosChanBlock::setChannelNumber(int value) {
    m_chanNumber = limit(1, value, 32768);
    emit channelNumberChanged();
    setValueText("");
}

void EosChanBlock::forwardDataToOutput() {
    // pass through data to output node:
    if (m_outputNode->isConnected()) {
        const ColorMatrix& input = m_inputNode->constData();
        auto out = HsvDataModifier(m_outputNode);
        if (input.width() < out.width + 1 || input.height() < out.height) {
            qWarning() << "FixtureBlock forward: data too small";
            return;
        }
        for (int x = 0; x < out.width; ++x) {
            for (int y = 0; y < out.height; ++y) {
                auto color = input.getHsvAt(x + 1, y);
                out.set(x, y, color.h, color.s, color.v);
            }
        }
    }
}

void EosChanBlock::onConnectionChanged() {
    if (m_outputNode->isConnected()) {
        // new requested size is requested size of previous node + 1 in x-direction
        Size requestedSize = m_outputNode->getRequestedSize();
        requestedSize.width += 1;
        m_inputNode->setRequestedSize(requestedSize);
        forwardDataToOutput();
    } else {
        // if ouput is not connected requested size is just one pixel:
        m_inputNode->setRequestedSize(Size(1, 1));
    }
}
