#include "KeyPressBlock.h"

#include "core/Nodes.h"
#include "core/MainController.h"
#include "core/manager/KeyboardEmulator.h"

#include <QKeyEvent>


KeyPressBlock::KeyPressBlock(MainController* controller, QString uid)
	: InOutBlock(controller, uid)
	, m_scanCode(this, "scanCode", 0, 0, INT_MAX)
	, m_fromKeyPad(this, "fromKeyPad", false)
	, m_keyName(this, "keyName", "")
{
	connect(&m_scanCode, &IntegerAttribute::valueChanged, this, &KeyPressBlock::keyNameChanged);

	m_inputNode->enableImpulseDetection();
	connect(m_inputNode, SIGNAL(impulseBegin()), this, SLOT(pressKey()));
	connect(m_inputNode, SIGNAL(impulseEnd()), this, SLOT(releaseKey()));
}

void KeyPressBlock::pressKey() {
	int scanCode = m_scanCode.getValue();
	bool extended = scanCode & 0b0000000100000000;
	scanCode &= 0b0000000011111111;
	m_controller->keyboardEmulator()->pressKey(static_cast<quint32>(scanCode), extended);
}

void KeyPressBlock::releaseKey() {
	int scanCode = m_scanCode.getValue();
	bool extended = scanCode & 0b0000000100000000;
	scanCode &= 0b0000000011111111;
	m_controller->keyboardEmulator()->releaseKey(static_cast<quint32>(scanCode), extended);
	QTimer::singleShot(50, [this](){ m_outputNode->sendImpulse(); });
}

QString KeyPressBlock::getKeyName() const {
	int scanCode = m_scanCode.getValue();
	if (scanCode == 328) {
		return "Up";
	} else if (scanCode == 336) {
		return "Down";
	} else if (scanCode == 331) {
		return "Left";
	} else if (scanCode == 333) {
		return "Right";
	} else if (scanCode == 327) {
		return "Pos1";
	} else if (scanCode == 335) {
		return "End";
	} else if (scanCode == 329) {
		return "Page ^";
	} else if (scanCode == 337) {
		return "Page v";
	} else if (scanCode == 338) {
		return "Insert";
	} else if (scanCode == 339) {
		return "Delete";
    } else if (scanCode == ScanCode::RETURN) {
		return "Return";
	} else if (!m_keyName.getValue().isEmpty()) {
		return m_keyName;
	} else {
		return "[" + QString::number(scanCode) + "]";
	}
}
