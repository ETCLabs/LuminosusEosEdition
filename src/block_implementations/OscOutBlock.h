#ifndef OSCOUTBLOCK_H
#define OSCOUTBLOCK_H

#include "block_data/OneInputBlock.h"

class OscOutBlock : public OneInputBlock
{
    Q_OBJECT

	Q_PROPERTY(QString message READ getMessage WRITE setMessage NOTIFY messageChanged)
	Q_PROPERTY(double minValue READ getMinValue WRITE setMinValue NOTIFY minValueChanged)
	Q_PROPERTY(double maxValue READ getMaxValue WRITE setMaxValue NOTIFY maxValueChanged)
	Q_PROPERTY(bool useInteger READ getUseInteger WRITE setUseInteger NOTIFY useIntegerChanged)

public:

	static BlockInfo info() {
		static BlockInfo info;
		info.typeName = "OSC Out";
        info.nameInUi = "OSC Output";
        info.category << "Custom OSC";
		info.helpText = "Enter message in the form:\n/path/to/parameter=argument\n\n"
						"<value> will be replaced with a value in the defined range\n"
                        "(see settings above)\n"
                        "An absolute value like a time value will not be scaled.\n"
                        "To convert a time (i.e. from BPM Block) to a real BPM value use <bpm>.";
		info.qmlFile = "qrc:/qml/Blocks/OscOutBlock.qml";
		info.complete<OscOutBlock>();
		return info;
	}

	explicit OscOutBlock(MainController* controller, QString uid);

	virtual QJsonObject getState() const override;
	virtual void setState(const QJsonObject& state) override;

signals:
	void messageChanged();
	void minValueChanged();
	void maxValueChanged();
	void useIntegerChanged();
	void messageSent();

public slots:
	virtual BlockInfo getBlockInfo() const override { return info(); }

	void onValueChanged();

	QString getMessage() const { return m_message; }
	void setMessage(const QString& value) { m_message = value; emit messageChanged(); }

	double getMinValue() const { return m_minValue; }
	void setMinValue(double value);

	double getMaxValue() const { return m_maxValue; }
	void setMaxValue(double value);

	bool getUseInteger() const { return m_useInteger; }
	void setUseInteger(bool value) { m_useInteger = value; emit useIntegerChanged(); }

protected:
	QString m_message;
	double m_lastValue;
	double m_minValue;
	double m_maxValue;
	bool m_useInteger;
};

#endif // OSCOUTBLOCK_H
