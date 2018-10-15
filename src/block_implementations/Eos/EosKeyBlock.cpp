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

#include "core/MainController.h"
#include "core/Nodes.h"


EosKeyBlock::EosKeyBlock(MainController* controller, QString uid)
	: OneInputBlock(controller, uid)
    , m_keyName("go_0")
	, m_keyNames()
	, m_keyLabels()
	, m_lastValue(0.0)
    , m_runInbackground(this, "runInBackground", false)
{
    addAllKeys();

	connect(m_inputNode, SIGNAL(dataChanged()), this, SLOT(onInputChanged()));
}

void EosKeyBlock::getAdditionalState(QJsonObject& state) const {
	state["keyName"] = getKeyName();
    state["customLabel"] = getCustomLabel();
    state["customKeyName"] = getCustomKeyName();
}

void EosKeyBlock::setAdditionalState(const QJsonObject &state) {
	setKeyName(state["keyName"].toString());
    setCustomLabel(state["customLabel"].toString());
    setCustomKeyName(state["customKeyName"].toString());
}

void EosKeyBlock::onInputChanged() {
    double value = m_inputNode->getValue();
    QQuickItem* button = getGuiItemChild("button");
    if (value >= LuminosusConstants::triggerThreshold && m_lastValue < LuminosusConstants::triggerThreshold) {
        if (button) button->setProperty("marked", true);
        onButtonPress();
    } else if (value < LuminosusConstants::triggerThreshold && m_lastValue >= LuminosusConstants::triggerThreshold) {
        if (button) button->setProperty("marked", false);
        onButtonRelease();
    }
	m_lastValue = value;
}

void EosKeyBlock::onButtonPress() {
    QString message = m_runInbackground ? "/eos/user/0/key/%1" : "/eos/user/1/key/%1";
    if (m_keyName == "custom") {
        message = message.arg(m_customKeyName);
    } else {
        message = message.arg(m_keyName);
    }
    m_controller->lightingConsole()->sendMessage(message, 1.0);
}

void EosKeyBlock::onButtonRelease() {
    QString message = m_runInbackground ? "/eos/user/0/key/%1" : "/eos/user/1/key/%1";
    if (m_keyName == "custom") {
        message = message.arg(m_customKeyName);
    } else {
        message = message.arg(m_keyName);
    }
    m_controller->lightingConsole()->sendMessage(message, 0.0);
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
    if (m_keyName == "custom") {
        return m_customLabel;
    }
	return m_keyLabels.value(m_keyName, "?");
}

void EosKeyBlock::setKeyName(QString value) {
	m_keyName = value;
    emit keyNameChanged();
}

void EosKeyBlock::setCustomLabel(QString value) {
    m_customLabel = value;
    emit customLabelChanged();
    emit keyNameChanged();  // to update button label
}

void EosKeyBlock::addAllKeys() {
    addKey("go_0", "GO");
    addKey("enter", "Enter");
    addKey("cue", "Cue");
    addKey("custom", "Custom");
}

void EosKeyBlock::addKey(QString keyName, QString label) {
	m_keyNames.append(keyName);
	m_keyLabels[keyName] = label;
}
