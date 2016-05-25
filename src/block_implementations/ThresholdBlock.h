#ifndef THRESHOLDBLOCK_H
#define THRESHOLDBLOCK_H

#include "block_data/InOutBlock.h"
#include "utils.h"


class ThresholdBlock : public InOutBlock
{
	Q_OBJECT

	Q_PROPERTY(qreal threshold READ getThreshold WRITE setThreshold NOTIFY thresholdChanged)
	Q_PROPERTY(qreal inputValue READ getInputValue WRITE setInputValue NOTIFY inputValueChanged)

public:

	static BlockInfo info() {
		static BlockInfo info;
		info.name = "Threshold";
		info.category = QStringList {"Logic"};
		info.helpText = "If the incoming value is below the threshold, the bottom output Node "
						"will be activated.\n"
						"If it is above the threshold the top one is activated.";
		info.qmlFile = "qrc:/qml/Blocks/ThresholdBlock.qml";
		info.complete<ThresholdBlock>();
		return info;
	}

	explicit ThresholdBlock(MainController* controller, QString uid);

	virtual QJsonObject getState() const override;
	virtual void setState(const QJsonObject& state) override;

signals:
	void thresholdChanged();
	void inputValueChanged();

public slots:
	virtual BlockInfo getBlockInfo() const override { return info(); }

	qreal getThreshold() const { return m_threshold; }
	void setThreshold(qreal value) { m_threshold = limit(0, value, 1); emit thresholdChanged(); }

	qreal getInputValue() const { return m_inputValue; }
	void setInputValue(qreal value) { m_inputValue = limit(0, value, 1); emit inputValueChanged(); }

private slots:
	void update();

protected:
	double		m_threshold;
	double		m_inputValue;
	OutputNodeHsv* m_outputNodeBelow;
};

#endif // THRESHOLDBLOCK_H
