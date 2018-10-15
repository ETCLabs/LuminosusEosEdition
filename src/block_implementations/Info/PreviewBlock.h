#ifndef PREVIEWBLOCK_H
#define PREVIEWBLOCK_H

#include "core/block_data/OneInputBlock.h"
#include "utils.h"
#include "core/QCircularBuffer.h"

#include <QTimer>


class PreviewBlock : public OneInputBlock
{
	Q_OBJECT

    Q_PROPERTY(QString valueString READ getValueString NOTIFY valueStringChanged)
    Q_PROPERTY(QVector<double> valueHistory READ getValueHistory NOTIFY valueHistoryChanged)

public:

	static BlockInfo info() {
		static BlockInfo info;
        info.typeName = "Value Inspector";
        info.category << "General";
        info.helpText = "Shows the incoming value in percent or as an absolute value.\n"
                        "Connect it in addition to other Blocks to an output to "
                        "preview its value.";
        info.qmlFile = "qrc:/qml/Blocks/Info/PreviewBlock.qml";
		info.complete<PreviewBlock>();
		return info;
	}

	explicit PreviewBlock(MainController* controller, QString uid);

signals:
    void valueStringChanged();
    void valueHistoryChanged();

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    QString getValueString() const { return m_valueString; }
    void setValueString(QString value) { m_valueString = value; emit valueStringChanged(); }

    QVector<double> getValueHistory() const;

private slots:
    void eachFrame();

protected:
    QString m_valueString;
    StringAttribute m_additionalValueString;
    DoubleAttribute m_currentValue;

    Qt3DCore::QCircularBuffer<double> m_valueHistory;

};

#endif // PREVIEWBLOCK_H
