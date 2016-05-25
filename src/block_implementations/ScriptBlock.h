#ifndef SCRIPTBLOCK_H
#define SCRIPTBLOCK_H

#include <QObject>
#include <QJSEngine>

#include "block_data/BlockBase.h"
#include "NodeBase.h"

class ScriptBlock : public BlockBase {

    Q_OBJECT

	Q_PROPERTY(QString code READ getCode WRITE setCode NOTIFY codeChanged)
	Q_PROPERTY(bool codeIsValid READ getCodeIsValid WRITE setCodeIsValid NOTIFY codeIsValidChanged)

public:

	static BlockInfo info() {
		static BlockInfo info;
		info.name = "Script";
		info.category = QStringList {"Logic"};
		info.helpText = "Enter JS code that evaluates to a number between 0 and 1.\n"
						"The variable 'value' contains the incoming value.\n\n"
						"Example:\nTo invert the value enter '1 - value'.\n"
						"To scale the value enter 'value * 0.3'.";
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

public slots:
	virtual BlockInfo getBlockInfo() const override { return info(); }

	void evaluateScript();

	QString getCode() const { return m_code; }
	void setCode(const QString& value) { m_code = value; emit codeChanged(); }

	bool getCodeIsValid() const { return m_codeIsValid; }
	void setCodeIsValid(bool value) { m_codeIsValid = value; emit codeIsValidChanged(); }

protected:
	QString m_code;
	bool m_codeIsValid;
	OutputNodeHsv* outputNode = nullptr;
	InputNodeHsv* inputNode = nullptr;
	QJSEngine jsEngine;

};

#endif // SCRIPTBLOCK_H
