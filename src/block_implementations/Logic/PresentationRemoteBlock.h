#ifndef PRESENTATIONREMOTEBLOCK_H
#define PRESENTATIONREMOTEBLOCK_H

#include "core/block_data/OneInputBlock.h"
#include "core/SmartAttribute.h"


class PresentationRemoteBlock : public OneInputBlock {

	Q_OBJECT

public:

	static BlockInfo info() {
		static BlockInfo info;
		info.typeName = "Presentation Remote Control";
		info.nameInUi = "PowerPoint Control";
        info.category << "General" << "Media + Presentation";
        info.visibilityRequirements << VisibilityRequirement::KeyboardEmulation;
		info.helpText = "Goes to the next or previous slide during a PowerPoint presentation.<br>"
						"Can also trigger a solid black or white slide.<br><br>"
						"Works by emulating the press of the page number and the enter key on the "
						"keyboard. This means that it only works while <b>PowerPoint is in the "
						"foreground!</b><br><br>"
						"Also works with LibreOffice Impress and other programs that support "
						"the keyboard shortcuts (arrow keys and 'W' / 'B').<br><br>"
						"The buttons can be mapped to a MIDI control as any other buttons.<br><br>"
						"Only works on Windows.";
		info.qmlFile = "qrc:/qml/Blocks/Logic/PresentationRemoteBlock.qml";
		info.complete<PresentationRemoteBlock>();
		return info;
	}

	explicit PresentationRemoteBlock(MainController* controller, QString uid);

public slots:
	virtual BlockInfo getBlockInfo() const override { return info(); }

	void nextSlide();
	void previousSlide();
	void whiteSlide();
	void blackSlide();

protected:
	NodeBase* m_prevNode;
	NodeBase* m_whiteNode;
	NodeBase* m_blackNode;
};

#endif // PRESENTATIONREMOTEBLOCK_H
