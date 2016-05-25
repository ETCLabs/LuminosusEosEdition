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

#include "ScriptBlock.h"

#include "NodeBase.h"


ScriptBlock::ScriptBlock(MainController *controller, QString uid)
	: BlockBase(controller, uid, info().qmlFile)
	, m_codeIsValid(true)
{
	// prepare nodes:
	outputNode = createOutputNodeHsv("outputNode");
	inputNode = createInputNodeHsv("inputNode");

	//connect signals and slots:
	connect(inputNode, SIGNAL(dataChanged()), this, SLOT(evaluateScript()));
	connect(this, SIGNAL(codeChanged()), this, SLOT(evaluateScript()));
}

QJsonObject ScriptBlock::getState() const {
	QJsonObject state;
	state["code"] = getCode();
	return state;
}

void ScriptBlock::setState(const QJsonObject &state) {
	setCode(state["code"].toString());
}

void ScriptBlock::evaluateScript() {
	QString js = m_code;
	jsEngine.globalObject().setProperty("value", inputNode->data->getValue());
	QJSValue result = jsEngine.evaluate(js);
	if (result.isError()) {
		qDebug()
				<< "[ScriptBlock] Uncaught exception at line"
				<< result.property("lineNumber").toInt()
				<< ":" << result.toString();
		setCodeIsValid(false);
		return;
	}
	if (!result.isNumber()) {
		qDebug() << "[ScriptBlock] Result is not a number.";
		setCodeIsValid(false);
		return;
	}
	setCodeIsValid(true);
	double value = result.toNumber();
	value = std::max(0.0, std::min(value, 1.0));
	outputNode->setValue(value);
}
