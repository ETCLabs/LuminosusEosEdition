#ifndef PREVIEWBLOCK_H
#define PREVIEWBLOCK_H

#include "block_data/OneInputBlock.h"
#include "utils.h"

#include <QTimer>


class PreviewBlock : public OneInputBlock
{
	Q_OBJECT

    Q_PROPERTY(qreal value READ getValue WRITE setValue NOTIFY valueChanged)

public:

	static BlockInfo info() {
		static BlockInfo info;
		info.typeName = "Value Preview";
		info.category << "Other";
        info.helpText = "Shows the incoming value in percent or as an absolute value.\n"
                        "Connect it in addition to other Blocks to an output node to "
                        "preview its value.";
		info.qmlFile = "qrc:/qml/Blocks/PreviewBlock.qml";
		info.complete<PreviewBlock>();
		return info;
	}

	explicit PreviewBlock(MainController* controller, QString uid);

signals:
	void valueChanged();

public slots:
	virtual BlockInfo getBlockInfo() const override { return info(); }

	void onValueChanged();

    qreal getValue() const { return m_value; }
	void setValue(qreal value) { m_value = value; emit valueChanged(); }

protected:
	qreal m_value;

};

#endif // PREVIEWBLOCK_H
