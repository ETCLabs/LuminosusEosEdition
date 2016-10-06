#ifndef SCRIPTBLOCK_H
#define SCRIPTBLOCK_H

#include "block_data/BlockBase.h"
#include "Nodes.h"

#include <QJSEngine>


class ScriptBlock : public BlockBase {

    Q_OBJECT

	Q_PROPERTY(QString code READ getCode WRITE setCode NOTIFY codeChanged)
	Q_PROPERTY(bool codeIsValid READ getCodeIsValid WRITE setCodeIsValid NOTIFY codeIsValidChanged)
    Q_PROPERTY(double variableX READ getVariableX WRITE setVariableX NOTIFY variableXChanged)
    Q_PROPERTY(double variableY READ getVariableY WRITE setVariableY NOTIFY variableYChanged)
    Q_PROPERTY(double variableZ READ getVariableZ WRITE setVariableZ NOTIFY variableZChanged)

public:

	static BlockInfo info() {
		static BlockInfo info;
        info.typeName = "Script";
        info.nameInUi = "Script / Formular";
        info.category << "Logic";
        info.helpText = "Enter JavaScript code like <b>'v = x * y * z'</b>.<br>"
                        "The input variables <i>x</i>, <i>y</i> and <i>z</i> are always between 0 and 1.<br>"
                        "The output variable <i>v</i> should also be in the range 0-1.<br><br>"
                        "Examples:<br><br>"
                        "- To invert the value of <i>x</i> enter <br><b>'v = 1 - x'</b>.<br><br>"
                        "- To scale the value of <i>y</i> enter <br><b>'v = y*0.3'</b>.<br><br>"
                        "- A condition would look like: <br><b>v = (x&lt;0.4) ? y : z</b><br><br>"
                        "- 'Lowest Takes Precedence' of <i>x</i> and <i>y</i> would be: <br><b>v = Math.min(x, y)</b>";
		info.qmlFile = "qrc:/qml/Blocks/ScriptBlock.qml";
		info.complete<ScriptBlock>();
		return info;
	}

	ScriptBlock(MainController* controller, QString uid);

	virtual QJsonObject getState() const override;
	virtual void setState(const QJsonObject& state) override;

signals:
	void codeChanged();
	void codeIsValidChanged();
    void variableXChanged();
    void variableYChanged();
    void variableZChanged();

public slots:
	virtual BlockInfo getBlockInfo() const override { return info(); }

    void onInputXChanged();
    void onInputYChanged();
    void onInputZChanged();

    void updateScriptFunction();

    void updateOutput();

	QString getCode() const { return m_code; }
    void setCode(const QString& value);

	bool getCodeIsValid() const { return m_codeIsValid; }
    void setCodeIsValid(bool value) { m_codeIsValid = value; emit codeIsValidChanged(); }

    double getVariableX() const { return m_variableX; }
    void setVariableX(double value) { m_variableX = value; emit variableXChanged(); updateOutput(); }

    double getVariableY() const { return m_variableY; }
    void setVariableY(double value) { m_variableY = value; emit variableYChanged(); updateOutput(); }

    double getVariableZ() const { return m_variableZ; }
    void setVariableZ(double value) { m_variableZ = value; emit variableZChanged(); updateOutput(); }

protected:
    QPointer<NodeBase> m_outputNode;
    QPointer<NodeBase> m_inputX;
    QPointer<NodeBase> m_inputY;
    QPointer<NodeBase> m_inputZ;

    QJSEngine m_jsEngine;
    QString m_codePrefix;
    QString m_codePostfix;
    QJSValue m_function;

    QString m_code;
    bool m_codeIsValid;
    double m_variableX;
    double m_variableY;
    double m_variableZ;

};

#endif // SCRIPTBLOCK_H
