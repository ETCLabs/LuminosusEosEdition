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

#include "EosKeyBlock.h"

#include "MainController.h"
#include "NodeBase.h"


EosKeyBlock::EosKeyBlock(MainController* controller, QString uid)
	: OneInputBlock(controller, uid, info().qmlFile)
	, m_keyName("go")
	, m_keyNames()
	, m_keyLabels()
	, m_lastValue(0.0)
{
	addKey("go", "GO");
	addKey("enter", "Enter");
	addKey("cue", "Cue");

	connect(m_inputNode, SIGNAL(dataChanged()), this, SLOT(onInputChanged()));
}

QJsonObject EosKeyBlock::getState() const {
	QJsonObject state;
	state["keyName"] = getKeyName();
	return state;
}

void EosKeyBlock::setState(const QJsonObject &state) {
	setKeyName(state["keyName"].toString());
}

void EosKeyBlock::onInputChanged() {
	double value = m_inputNode->data->getValue();
	// FIXME: this depends on the GUI
	QQuickItem* button = getGuiItemChild("button");
	if (value >= 0.5 && m_lastValue < 0.5) {
		QMetaObject::invokeMethod(button, "handleTouchDown");
	} else if (value < 0.5 && m_lastValue >= 0.5) {
		QMetaObject::invokeMethod(button, "handleTouchUp");
	}
	m_lastValue = value;
}

void EosKeyBlock::onButtonPress() {
	QString message = "/eos/key/%1=1";
	message = message.arg(m_keyName);
	m_controller->osc()->sendMessage(message);
}

void EosKeyBlock::onButtonRelease() {
	QString message = "/eos/key/%1=0";
	message = message.arg(m_keyName);
	m_controller->osc()->sendMessage(message);
}

QStringList EosKeyBlock::getLabelTexts() const {
	QStringList labels;
	for (QString keyName: m_keyNames) {
		labels.append(m_keyLabels.value(keyName, "?"));
	}
	return labels;
}

QStringList EosKeyBlock::getKeyNames() const {
	return m_keyNames;
}

QString EosKeyBlock::getLabelText() const {
	return m_keyLabels.value(m_keyName, "?");
}

void EosKeyBlock::setKeyName(QString value) {
	m_keyName = value;
	emit keyNameChanged();
}

void EosKeyBlock::addKey(QString keyName, QString label) {
	m_keyNames.append(keyName);
	m_keyLabels[keyName] = label;
}
