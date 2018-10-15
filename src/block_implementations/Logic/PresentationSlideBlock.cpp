#include "PresentationSlideBlock.h"

#include "core/MainController.h"
#include "core/manager/KeyboardEmulator.h"
#include "core/Nodes.h"

#include <QQuickWindow>


PresentationSlideBlock::PresentationSlideBlock(MainController* controller, QString uid)
	: OneInputBlock(controller, uid)
	, m_slideNumber(this, "slideNumber", 1, 1, 999)
{
	m_inputNode->enableImpulseDetection();
	connect(m_inputNode, SIGNAL(impulseBegin()), this, SLOT(goToSlide()));
}

void PresentationSlideBlock::goToSlide() {
    if (m_controller->guiManager()->getMainWindow()->isActive()) {
        // PowerPoint is not in foreground (but Luminosus)
        // -> don't send key
        return;
    }
	QByteArray chars = QString::number(m_slideNumber).toLatin1();
	if (chars.length() <= 0) return;
	QVector<quint32> scanCodes;
	for (char c: chars) {
		switch (c) {
        case '0': scanCodes.append(ScanCode::NUMBER_0); break;
        case '1': scanCodes.append(ScanCode::NUMBER_1); break;
        case '2': scanCodes.append(ScanCode::NUMBER_2); break;
        case '3': scanCodes.append(ScanCode::NUMBER_3); break;
        case '4': scanCodes.append(ScanCode::NUMBER_4); break;
        case '5': scanCodes.append(ScanCode::NUMBER_5); break;
        case '6': scanCodes.append(ScanCode::NUMBER_6); break;
        case '7': scanCodes.append(ScanCode::NUMBER_7); break;
        case '8': scanCodes.append(ScanCode::NUMBER_8); break;
        case '9': scanCodes.append(ScanCode::NUMBER_9); break;
		default: break;
		}
    }
	pressNextKey(scanCodes, 0);
}

void PresentationSlideBlock::pressNextKey(QVector<quint32> scanCodes, int index) {
    if (scanCodes.length() <= index) {
        m_controller->keyboardEmulator()->pressKey(ScanCode::RETURN);
        QTimer::singleShot(50, [this](){ m_controller->keyboardEmulator()->releaseKey(ScanCode::RETURN); });
		return;
    }
	m_controller->keyboardEmulator()->pressKey(static_cast<quint32>(scanCodes.at(index)));
	QTimer::singleShot(50, [this, scanCodes, index](){ m_controller->keyboardEmulator()->releaseKey(static_cast<quint32>(scanCodes.at(index))); });
	QTimer::singleShot(100, [this, scanCodes, index](){ pressNextKey(scanCodes, index + 1); });
}
