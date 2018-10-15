#ifndef PRESENTATIONSLIDEBLOCK_H
#define PRESENTATIONSLIDEBLOCK_H

#include "core/block_data/OneInputBlock.h"
#include "core/SmartAttribute.h"


class PresentationSlideBlock : public OneInputBlock {

	Q_OBJECT

public:

	static BlockInfo info() {
		static BlockInfo info;
		info.typeName = "Presentation Slide";
		info.nameInUi = "PowerPoint Slide";
        info.category << "General" << "Media + Presentation";
        info.visibilityRequirements << VisibilityRequirement::KeyboardEmulation;
		info.helpText = "Goes to a specific slide during a PowerPoint presentation.<br><br>"
						"Works by emulating the press of the page number and the enter key on the "
						"keyboard. This means that it only works while <b>PowerPoint is in the "
						"foreground!</b><br><br>"
						"Also works with LibreOffice Impress and other programs that support "
						"this keyboard combination.<br><br>"
						"The 'PowerPoint Remote Block' can be used to go to the next or "
						"previous slide and to show black and white slides.<br>"
						"The 'Fire' button can be mapped to a MIDI control as any other button.<br><br>"
						"Only works on Windows.";
		info.qmlFile = "qrc:/qml/Blocks/Logic/PresentationSlideBlock.qml";
		info.complete<PresentationSlideBlock>();
		return info;
	}

	explicit PresentationSlideBlock(MainController* controller, QString uid);

signals:

public slots:
	virtual BlockInfo getBlockInfo() const override { return info(); }

	void goToSlide();

private:
	void pressNextKey(QVector<quint32> scanCodes, int index);

protected:
	IntegerAttribute m_slideNumber;
};

#endif // PRESENTATIONSLIDEBLOCK_H
