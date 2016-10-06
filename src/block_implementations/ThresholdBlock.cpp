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

#include "Nodes.h"


ThresholdBlock::ThresholdBlock(MainController* controller, QString uid)
	: InOutBlock(controller, uid, info().qmlFile)
	, m_threshold(0.5)
{
	m_outputNodeBelow = createOutputNode("outputNodeBelow");
	connect(m_inputNode, SIGNAL(dataChanged()), this, SLOT(update()));
	connect(this, SIGNAL(thresholdChanged()), this, SLOT(update()));
}

QJsonObject ThresholdBlock::getState() const {
	QJsonObject state;
	state["threshold"] = getThreshold();
	return state;
}

void ThresholdBlock::setState(const QJsonObject &state) {
	setThreshold(state["threshold"].toDouble());
}

void ThresholdBlock::update() {
    double value = m_inputNode->getValue();
	if (value > m_threshold) {
		m_outputNode->setValue(1.0);
		m_outputNodeBelow->setValue(0.0);
	} else {
		m_outputNode->setValue(0.0);
		m_outputNodeBelow->setValue(1.0);
	}
	setInputValue(value);
}
