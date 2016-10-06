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

#include "DimmerBlock.h"

#include "MainController.h"
#include "Nodes.h"

int DimmerBlock::instances = 0;

DimmerBlock::DimmerBlock(MainController *controller, QString uid)
	: BlockBase(controller, uid, info().qmlFile)
{
	// prepare nodes:
	outputNode = createOutputNode("outputNode");
	inputNode = createInputNode("inputNode");
	// prepare member variables:
	instances++;
	m_channel = instances;

	//connect signals and slots:
	connect(inputNode, SIGNAL(dataChanged()), this, SIGNAL(valueChanged()));
	connect(this, SIGNAL(valueChanged()), this, SLOT(onValueChanged()));
}

QJsonObject DimmerBlock::getState() const {
	QJsonObject state;
	state["name"] = getName();
	state["channel"] = getChannel();
	state["value"] = getValue();
	return state;
}

void DimmerBlock::setState(const QJsonObject &state) {
	setName(state["name"].toString());
	setChannel(state["channel"].toInt());
	setValue(state["value"].toDouble());
}

double DimmerBlock::getValue() const {
	if (inputNode) {
        return inputNode->getValue();
	} else {
		return 0;
	}
}

void DimmerBlock::setChannel(int channel) {
	if (!(1 <= channel && channel <= 8192)) return;
	m_channel = channel; channelChanged();
}

void DimmerBlock::onValueChanged()
{
    double value = inputNode->getValue();
	m_controller->output()->setChannel(m_channel, value);
	outputNode->setValue(value);
}
