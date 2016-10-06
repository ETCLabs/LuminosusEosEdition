#include "AnchorManager.h"

#include "MainController.h"


AnchorManager::AnchorManager(MainController* controller)
    : m_controller(controller)
    , m_presentationMode(false)
    , m_currentIndex(0)
{

}

QJsonObject AnchorManager::getState() const {
    QJsonObject state;
    QJsonArray anchorOrder;
    for (QString uid: m_anchorOrder) {
        anchorOrder.append(uid);
    }
    state["anchorOrder"] = anchorOrder;
    return state;
}

void AnchorManager::setState(const QJsonObject& state) {
    QJsonArray anchorOrder = state["anchorOrder"].toArray();
    m_anchorOrder.clear();
    for (QJsonValueRef value: anchorOrder) {
        m_anchorOrder.append(value.toString());
    }
}

void AnchorManager::showNext() {
    if (m_currentIndex >= m_anchors.size() - 1) {
        // current anchor is last anchor
        return;
    }
    setCurrentIndex(m_currentIndex + 1);
    showCurrent();
}

void AnchorManager::showCurrent() {
    if (m_anchors.isEmpty()) return;

    if (m_currentIndex < 0) {
        setCurrentIndex(0);
    } else if (m_currentIndex >= m_anchors.size()) {
        setCurrentIndex(m_anchors.size() - 1);
    }

    QString uid = m_anchorOrder[m_currentIndex];
    if (!m_anchors.contains(uid)) {
        qWarning() << "Anchor Order is not correct.";
        m_anchorOrder.removeAll(uid);
        return showCurrent();
    }

    BlockInterface* block = m_anchors[uid];
    if (!block) {
        qWarning() << "Could not get current anchor block.";
        return;
    }

    QQuickWindow* mainWindow = m_controller->getMainWindow();
    if (!mainWindow) return;
    double dp = m_controller->getGuiScaling();

    qreal pageCenterX = block->getGuiItem()->x() - 15*dp + (1280*dp / 2.0);
    qreal pageCenterY = block->getGuiItem()->y() - 15*dp + (720*dp / 2.0);
    qreal x = pageCenterX - mainWindow->width() / 2.0;
    qreal y = pageCenterY - mainWindow->height() / 2.0;

    m_controller->projectManager()->setPlanePosition(-x, -y);
}

void AnchorManager::showPrevious() {
    if (m_currentIndex <= 0) {
        // current anchor is first anchor
        return;
    }
    setCurrentIndex(m_currentIndex - 1);
    showCurrent();
}

void AnchorManager::startPresentationMode() {
    m_controller->setKeyboardFocusToPlane();
    setPresentationMode(true);
    showCurrent();
}

void AnchorManager::endPresentationMode() {
    setPresentationMode(false);
}

void AnchorManager::togglePresentationMode() {
    if (getPresentationMode()) {
        endPresentationMode();
    } else {
        startPresentationMode();
    }
}

// ----------------------------------------------------------------------

void AnchorManager::addAnchor(BlockInterface* block) {
    if (m_anchors.contains(block->getUid())) {
        qWarning() << "Anchor is already registered.";
        return;
    }
    m_anchors[block->getUid()] = block;
    if (!m_anchorOrder.contains(block->getUid())) {
        // this is a new (not restored) block:
        m_anchorOrder.append(block->getUid());
    }
    emit anchorsChanged();
}

void AnchorManager::removeAnchor(BlockInterface* block) {
    if (!m_anchors.contains(block->getUid())) {
        qWarning() << "Tried to remove unknown anchor.";
        return;
    }
    m_anchors.remove(block->getUid());
    m_anchorOrder.removeAll(block->getUid());

    if (m_currentIndex >= m_anchors.size()) {
        setCurrentIndex(m_anchors.size() -1);
    }
    emit anchorsChanged();

    if (m_anchors.isEmpty()) {
        setPresentationMode(false);
    }
}

// ----------------------------------------------------------------------

int AnchorManager::getAnchorIndex(BlockInterface* block) {
    if (!m_anchorOrder.contains(block->getUid())) {
        return -1;
    }
    return m_anchorOrder.indexOf(block->getUid());
}
