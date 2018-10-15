#ifndef THRESHOLDBLOCK_H
#define THRESHOLDBLOCK_H

#include "core/block_data/InOutBlock.h"
#include "utils.h"


class ThresholdBlock : public InOutBlock
{
	Q_OBJECT

	Q_PROPERTY(qreal threshold READ getThreshold WRITE setThreshold NOTIFY thresholdChanged)
	Q_PROPERTY(qreal inputValue READ getInputValue WRITE setInputValue NOTIFY inputValueChanged)

public:

	static BlockInfo info() {
		static BlockInfo info;
		info.typeName = "Threshold";
        info.category << "General" << "Modify Value";
        info.helpText = "If the incoming value is above the threshold, the top output "
                        "will be activated / set to 1.\n"
                        "If it is below the threshold the bottom one is activated.\n\n"
                        "Useful to trigger actions from a continous value (i.e. trigger cues "
                        "from a volume level).";
        info.qmlFile = "qrc:/qml/Blocks/Logic/ThresholdBlock.qml";
		info.complete<ThresholdBlock>();
		return info;
	}

	explicit ThresholdBlock(MainController* controller, QString uid);

	virtual void getAdditionalState(QJsonObject& state) const override;
	virtual void setAdditionalState(const QJsonObject& state) override;

signals:
	void thresholdChanged();
	void inputValueChanged();

public slots:
	virtual BlockInfo getBlockInfo() const override { return info(); }

	qreal getThreshold() const { return m_threshold; }
	void setThreshold(qreal value) { m_threshold = limit(0, value, 1); emit thresholdChanged(); }

	qreal getInputValue() const { return m_inputValue; }
	void setInputValue(qreal value) { m_inputValue = limit(0, value, 1); emit inputValueChanged(); }

    virtual void onControllerRotated(double relativeAmount, double, bool) override { setThreshold(m_threshold + relativeAmount); }

private slots:
    void updateOutput();
    void updateRequestedSize();

protected:
	double		m_threshold;
	double		m_inputValue;
    NodeBase* m_outputNodeBelow;
};

#endif // THRESHOLDBLOCK_H
