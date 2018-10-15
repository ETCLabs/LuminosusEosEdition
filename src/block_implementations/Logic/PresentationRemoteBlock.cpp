#include "PresentationRemoteBlock.h"

#include "core/MainController.h"
#include "core/manager/KeyboardEmulator.h"
#include "core/Nodes.h"

#include <QQuickWindow>


PresentationRemoteBlock::PresentationRemoteBlock(MainController* controller, QString uid)
	: OneInputBlock(controller, uid)
	, m_prevNode(nullptr)
	, m_whiteNode(nullptr)
	, m_blackNode(nullptr)
{
	m_prevNode = createInputNode("prev");
	m_whiteNode = createInputNode("white");
	m_blackNode = createInputNode("black");

	m_inputNode->enableImpulseDetection();
	connect(m_inputNode, &NodeBase::impulseBegin, this, &PresentationRemoteBlock::nextSlide);
	m_prevNode->enableImpulseDetection();
	connect(m_prevNode, &NodeBase::impulseBegin, this, &PresentationRemoteBlock::previousSlide);
	m_whiteNode->enableImpulseDetection();
	connect(m_whiteNode, &NodeBase::impulseBegin, this, &PresentationRemoteBlock::whiteSlide);
	m_blackNode->enableImpulseDetection();
	connect(m_blackNode, &NodeBase::impulseBegin, this, &PresentationRemoteBlock::blackSlide);
}

void PresentationRemoteBlock::nextSlide() {
    if (m_controller->guiManager()->getMainWindow()->isActive()) {
        // PowerPoint is not in foreground (but Luminosus)
        // -> don't send key
        return;
    }
    m_controller->keyboardEmulator()->pressKey(ScanCode::RIGHT, true);
    QTimer::singleShot(50, [this](){ m_controller->keyboardEmulator()->releaseKey(ScanCode::RIGHT, true); });
}

void PresentationRemoteBlock::previousSlide() {
    if (m_controller->guiManager()->getMainWindow()->isActive()) {
        // PowerPoint is not in foreground (but Luminosus)
        // -> don't send key
        return;
    }
    m_controller->keyboardEmulator()->pressKey(ScanCode::LEFT, true);
    QTimer::singleShot(50, [this](){ m_controller->keyboardEmulator()->releaseKey(ScanCode::LEFT, true); });
}

void PresentationRemoteBlock::whiteSlide() {
    if (m_controller->guiManager()->getMainWindow()->isActive()) {
        // PowerPoint is not in foreground (but Luminosus)
        // -> don't send key
        return;
    }
    m_controller->keyboardEmulator()->pressKey(ScanCode::W, false);
    QTimer::singleShot(50, [this](){ m_controller->keyboardEmulator()->releaseKey(ScanCode::W, false); });
}

void PresentationRemoteBlock::blackSlide() {
    if (m_controller->guiManager()->getMainWindow()->isActive()) {
        // PowerPoint is not in foreground (but Luminosus)
        // -> don't send key
        return;
    }
    m_controller->keyboardEmulator()->pressKey(ScanCode::B, false);
    QTimer::singleShot(50, [this](){ m_controller->keyboardEmulator()->releaseKey(ScanCode::B, false); });
}
