#ifndef KEYPRESSBLOCK_H
#define KEYPRESSBLOCK_H

#include "core/block_data/InOutBlock.h"
#include "core/SmartAttribute.h"


class KeyPressBlock : public InOutBlock
{
	Q_OBJECT

	Q_PROPERTY(QString keyName READ getKeyName NOTIFY keyNameChanged)

public:

	static BlockInfo info() {
		static BlockInfo info;
		info.typeName = "Key Press";
        info.nameInUi = "Simulate Key Press";
        info.category << "General" << "Other";
        info.visibilityRequirements << VisibilityRequirement::KeyboardEmulation;
		info.helpText = "Emulates the press of a keyboard button.\n"
						"Choose a key by pressing it after clicking on the 'Key: ?' button.\n"
						"The 'Fire' button can be mapped to a MIDI control as any other button.\n\n"
						"Useful to interact with other programms while they are in the foreground.\n"
						"An example would be to control a PowerPoint presentation or media player.\n"
						"Multiple of this blocks can be daisy-chained. The next one will be pressed "
						"after the previous one was released.\n\n"
						"Only works on Windows.";
		info.qmlFile = "qrc:/qml/Blocks/Logic/KeyPressBlock.qml";
		info.complete<KeyPressBlock>();
		return info;
	}

	explicit KeyPressBlock(MainController* controller, QString uid);

signals:
	void keyNameChanged();

public slots:
	virtual BlockInfo getBlockInfo() const override { return info(); }

	void pressKey();

	void releaseKey();

	QString getKeyName() const;

protected:
	IntegerAttribute m_scanCode;
	BoolAttribute m_fromKeyPad;
	StringAttribute m_keyName;
};

#endif // KEYPRESSBLOCK_H
