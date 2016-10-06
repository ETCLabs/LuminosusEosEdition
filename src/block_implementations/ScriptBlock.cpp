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

#include "utils.h"


ScriptBlock::ScriptBlock(MainController *controller, QString uid)
	: BlockBase(controller, uid, info().qmlFile)
    , m_outputNode(nullptr)
    , m_inputX(nullptr)
    , m_inputY(nullptr)
    , m_inputZ(nullptr)
    , m_code("")
    , m_codeIsValid(false)
    , m_variableX(0.0)
    , m_variableY(0.0)
    , m_variableZ(0.0)
{
	// prepare nodes:
	m_outputNode = createOutputNode("outputNode");
    m_inputX = createInputNode("inputX");
    m_inputY = createInputNode("inputY");
    m_inputZ = createInputNode("inputZ");

	//connect signals and slots:
    connect(m_inputX, SIGNAL(dataChanged()), this, SLOT(onInputXChanged()));
    connect(m_inputY, SIGNAL(dataChanged()), this, SLOT(onInputYChanged()));
    connect(m_inputZ, SIGNAL(dataChanged()), this, SLOT(onInputZChanged()));

    // JS attributes:
    m_codePrefix = "(function(x, y, z) { var v = 0.0; ";
    m_codePostfix = "; return v; })";
    updateScriptFunction();
}

QJsonObject ScriptBlock::getState() const {
	QJsonObject state;
	state["code"] = getCode();
	return state;
}

void ScriptBlock::setState(const QJsonObject &state) {
    setCode(state["code"].toString());
}

void ScriptBlock::onInputXChanged() {
    setVariableX(m_inputX->getValue());
}

void ScriptBlock::onInputYChanged() {
    setVariableY(m_inputY->getValue());
}

void ScriptBlock::onInputZChanged() {
    setVariableZ(m_inputZ->getValue());
}

void ScriptBlock::updateScriptFunction() {
    QString js = m_codePrefix + m_code + m_codePostfix;
	QJSValue result = m_jsEngine.evaluate(js);
	if (result.isError()) {
		qDebug()
				<< "[ScriptBlock] Uncaught exception at line"
				<< result.property("lineNumber").toInt()
				<< ":" << result.toString();
		setCodeIsValid(false);
		return;
	}
    if (!result.isCallable()) {
        qDebug() << "[ScriptBlock] Function is not callable.";
		setCodeIsValid(false);
		return;
	}
    setCodeIsValid(true);
    m_function = result;
}

void ScriptBlock::updateOutput() {
    if (!m_codeIsValid) return;
    QJSValueList args;
    args << m_variableX << m_variableY << m_variableZ;
    QJSValue result = m_function.call(args);
    if (result.isError()) {
        qDebug()
                << "[ScriptBlock] Out: Uncaught exception at line"
                << result.property("lineNumber").toInt()
                << ":" << result.toString();
        return;
    }
    if (!result.isNumber()) {
        qDebug() << "[ScriptBlock] Out: Result is not a number.";
        return;
    }
    double value = limit(0, result.toNumber(), 1);
    m_outputNode->setValue(value);
}

void ScriptBlock::setCode(const QString& value) {
    m_code = value;
    emit codeChanged();
    updateScriptFunction();
    updateOutput();
}
