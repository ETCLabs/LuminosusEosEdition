#include "TouchArea.h"

#include <QQmlEngine>


TouchAreaEvent::TouchAreaEvent()
    : QObject(0)
    , m_isValid(false)
    , m_x(0)
    , m_y(0)
    , m_originX(0)
    , m_originY(0)
    , m_itemX(0)
    , m_itemY(0)
    , m_itemOriginX(0)
    , m_itemOriginY(0)
    , m_sceneX(0)
    , m_sceneY(0)
    , m_deltaX(0)
    , m_deltaY(0)
    , m_isTouch(false)
    , m_isRightButton(false)
    , m_modifiers(Qt::NoModifier)
    , m_id(-1)
    , m_accepted(true)
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}

TouchAreaEvent::TouchAreaEvent(TouchArea* touchArea)
    : QObject(touchArea)
    , m_isValid(false)
    , m_x(0)
    , m_y(0)
    , m_originX(0)
    , m_originY(0)
    , m_itemX(0)
    , m_itemY(0)
    , m_itemOriginX(0)
    , m_itemOriginY(0)
    , m_sceneX(0)
    , m_sceneY(0)
    , m_deltaX(0)
    , m_deltaY(0)
    , m_isTouch(false)
    , m_isRightButton(false)
    , m_modifiers(Qt::NoModifier)
    , m_id(-1)
    , m_accepted(true)
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}

TouchAreaEvent::TouchAreaEvent(const TouchAreaEvent& other)
    : QObject(other.parent())
    , m_isValid(other.m_isValid)
    , m_x(other.m_x)
    , m_y(other.m_y)
    , m_originX(other.m_originX)
    , m_originY(other.m_originY)
    , m_itemX(other.m_itemX)
    , m_itemY(other.m_itemY)
    , m_itemOriginX(other.m_itemOriginX)
    , m_itemOriginY(other.m_itemOriginY)
    , m_sceneX(other.m_sceneX)
    , m_sceneY(other.m_sceneY)
    , m_deltaX(other.m_deltaX)
    , m_deltaY(other.m_deltaY)
    , m_isTouch(other.m_isTouch)
    , m_isRightButton(other.m_isRightButton)
    , m_modifiers(other.m_modifiers)
    , m_id(other.m_id)
    , m_accepted(other.m_accepted)
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}

TouchAreaEvent::TouchAreaEvent(const QTouchEvent::TouchPoint& touchPoint, Qt::KeyboardModifiers modifiers, TouchArea* touchArea)
    : QObject(touchArea)
    , m_isValid(true)
    , m_x(touchPoint.screenPos().x())
    , m_y(touchPoint.screenPos().y())
    , m_originX(touchPoint.startScreenPos().x())
    , m_originY(touchPoint.startScreenPos().y())
    , m_itemX(touchPoint.pos().x())
    , m_itemY(touchPoint.pos().y())
    , m_itemOriginX(touchPoint.startPos().x())
    , m_itemOriginY(touchPoint.startPos().y())
    , m_sceneX(touchPoint.scenePos().x())
    , m_sceneY(touchPoint.scenePos().y())
    , m_deltaX(m_x - touchPoint.lastScreenPos().x())
    , m_deltaY(m_y - touchPoint.lastScreenPos().y())
    , m_isTouch(true)
    , m_isRightButton(false)
    , m_modifiers(modifiers)
    , m_id(touchPoint.id())
    , m_accepted(true)
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}

TouchAreaEvent::TouchAreaEvent(const QMouseEvent* mouseEvent, TouchArea* touchArea)
    : QObject(touchArea)
    , m_isValid(true)
    , m_x(mouseEvent->globalX())
    , m_y(mouseEvent->globalY())
    , m_originX(m_x)
    , m_originY(m_y)
    , m_itemX(mouseEvent->localPos().x())
    , m_itemY(mouseEvent->localPos().y())
    , m_itemOriginX(m_itemX)
    , m_itemOriginY(m_itemY)
    , m_sceneX(mouseEvent->windowPos().x())
    , m_sceneY(mouseEvent->windowPos().y())
    , m_deltaX(0.0)
    , m_deltaY(0.0)
    , m_isTouch(false)
    , m_isRightButton(mouseEvent->button() & Qt::RightButton)
    , m_modifiers(mouseEvent->modifiers())
    , m_id(TouchAreaConstants::mouseEventId)
    , m_accepted(true)
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}

TouchAreaEvent::TouchAreaEvent(const QMouseEvent* mouseEvent, const TouchAreaEvent& mouseLast, TouchArea* touchArea)
    : QObject(touchArea)
    , m_isValid(true)
    , m_x(mouseEvent->globalX())
    , m_y(mouseEvent->globalY())
    , m_originX(mouseLast.m_originX)
    , m_originY(mouseLast.m_originY)
    , m_itemX(mouseEvent->localPos().x())
    , m_itemY(mouseEvent->localPos().y())
    , m_itemOriginX(mouseLast.m_itemOriginX)
    , m_itemOriginY(mouseLast.m_itemOriginY)
    , m_sceneX(mouseEvent->windowPos().x())
    , m_sceneY(mouseEvent->windowPos().y())
    , m_deltaX(m_x - mouseLast.m_x)
    , m_deltaY(m_y - mouseLast.m_y)
    , m_isTouch(false)
    , m_isRightButton(mouseEvent->button() & Qt::RightButton)
    , m_modifiers(mouseEvent->modifiers())
    , m_id(TouchAreaConstants::mouseEventId)
    , m_accepted(true)
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}

const TouchAreaEvent& TouchAreaEvent::operator=(const TouchAreaEvent& other) {
    m_isValid = other.m_isValid;
    m_x = other.m_x;
    m_y = other.m_y;
    m_originX = other.m_originX;
    m_originY = other.m_originY;
    m_itemX = other.m_itemX;
    m_itemY = other.m_itemY;
    m_itemOriginX = other.m_itemOriginX;
    m_itemOriginY = other.m_itemOriginY;
    m_sceneX = other.m_sceneX;
    m_sceneX = other.m_sceneY;
    m_deltaX = other.m_deltaX;
    m_deltaY = other.m_deltaY;
    m_isTouch = other.m_isTouch;
    m_isRightButton = other.m_isRightButton;
    m_modifiers = other.m_modifiers;
    m_id = other.m_id;
    m_accepted = other.m_accepted;
    return *this;
}

void TouchAreaEvent::invalidate() {
    m_isValid = false;
    m_id = -1;
    emit isValidChanged();
}

bool TouchAreaEvent::isAtOrigin(qreal tolerance) const {
    QVector2D current(m_x, m_y);
    QVector2D origin(m_originX, m_originY);
    qreal dist = qAbs(current.distanceToPoint(origin));
    return dist < tolerance;
}

// -------------------------------------------------------------------------

TouchArea::TouchArea(QQuickItem* parent)
    : QQuickItem(parent)
    , m_pressed(false)
    , m_mouseOver(false)
    , m_clickDurationEnabled(false)
    , m_longClickDuration(TouchAreaConstants::defaultLongClickDuration)
    , m_secondTouchEnabled(false)
    , m_firstTouch(this)
    , m_secondTouch(this)
    , m_lastClickTimestamp(HighResTime::now())
    , m_scrollEventAccepted(false)
{
    m_longClickTimer.setSingleShot(true);
    connect(&m_longClickTimer, SIGNAL(timeout()), this, SLOT(onLongClickTimeout()));

    setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton);
    bool preventParentFromStealing = false;
    setKeepMouseGrab(preventParentFromStealing);
    setKeepTouchGrab(preventParentFromStealing);
}

void TouchArea::onLongClickTimeout() {
    if (m_firstTouch.isAtOrigin(TouchAreaConstants::defaultIsAtOriginTolerance * 2)) {
        emit longClick(&m_firstTouch);
    }
}

// --------------- Mouse and Touch movement ---------------------

void TouchArea::mousePressEvent(QMouseEvent* event) {
    TouchAreaEvent touch(event, this);
    m_mouseLast = touch;
    onTouchDownInternal(touch);
    event->setAccepted(touch.isAccepted());
}

void TouchArea::mouseMoveEvent(QMouseEvent* event) {
    TouchAreaEvent touch(event, m_mouseLast, this);
    m_mouseLast = touch;
    onTouchMoveInternal(touch);
    event->setAccepted(touch.isAccepted());
}

void TouchArea::mouseReleaseEvent(QMouseEvent* event) {
    TouchAreaEvent touch(event, m_mouseLast, this);
    onTouchUpInternal(touch);
    event->setAccepted(touch.isAccepted());
}

void TouchArea::mouseUngrabEvent() {
    // Mouse grab has been canceled
    // i.e. because of a flickable stealing the grab for a flick gesture
    if (m_secondTouch.isValid()) {
        onTouchUpInternal(m_secondTouch, true);
    }
    if (m_firstTouch.isValid()) {
        onTouchUpInternal(m_firstTouch, true);
    }
    emit touchCanceled();
}

void TouchArea::touchEvent(QTouchEvent* event) {
    if (event->type() == QEvent::TouchCancel) {
        qWarning() << "QEvent::TouchCancel received.";
        if (m_secondTouch.isValid()) {
            onTouchUpInternal(m_secondTouch, true);
        }
        if (m_firstTouch.isValid()) {
            onTouchUpInternal(m_firstTouch, true);
        }
        return;
    }
    QVector<int> grabIDs;
    bool accepted = false;
    for (const QTouchEvent::TouchPoint& touchPoint: event->touchPoints()) {
        TouchAreaEvent touch(touchPoint, event->modifiers(), this);
        switch (touchPoint.state()) {
        case Qt::TouchPointPressed:
            onTouchDownInternal(touch);
            if (touch.isAccepted()) {
                grabIDs.append(touchPoint.id());
            }
            break;
        case Qt::TouchPointMoved:
            onTouchMoveInternal(touch);
            break;
        case Qt::TouchPointStationary:
            break;
        case Qt::TouchPointReleased:
            onTouchUpInternal(touch);
            break;
        default:
            qWarning() << "Unkown Touch state received.";
            break;
        }
        accepted = accepted || touch.isAccepted();
    }
    event->setAccepted(accepted);
    grabTouchPoints(grabIDs);
}

void TouchArea::touchUngrabEvent() {
    // Touch grab has been canceled
    // i.e. because of a flickable stealing the grab for a flick gesture
    if (m_secondTouch.isValid()) {
        onTouchUpInternal(m_secondTouch, true);
    }
    if (m_firstTouch.isValid()) {
        onTouchUpInternal(m_firstTouch, true);
    }
    emit touchCanceled();
}

void TouchArea::onTouchDownInternal(TouchAreaEvent& touch) {
    if (!m_firstTouch.isValid()) {
        // this is the only touch point
        m_firstTouch = touch;
        setPressed(true);
        if (m_clickDurationEnabled && !touch.isRightButton()) {
            m_longClickTimer.start(m_longClickDuration);
        }
        emit touchDown(&touch, false);
    } else if (m_secondTouchEnabled && !m_secondTouch.isValid()) {
        // this is the second touch point
        m_secondTouch = touch;
        emit touchDown(&touch, true);
    } else {
        // this is another touch point after the second one
        // or it is the second and secondTouchEnabled is false
        // ignore it:
        touch.setAccepted(false);
    }
}

void TouchArea::onTouchMoveInternal(TouchAreaEvent& touch) {
    if (touch == m_firstTouch) {
        // this is the first touch point
        // update it:
        m_firstTouch = touch;
        emit touchMove(&touch, false);
    } else if (touch == m_secondTouch) {
        // this is the second touch point
        // update it:
        m_secondTouch = touch;
        emit touchMove(&touch, true);
    } else {
        // this is an unknown touch point
        // ignore it:
        touch.setAccepted(false);
    }
}

void TouchArea::onTouchUpInternal(TouchAreaEvent& touch, bool canceled) {
    if (touch == m_firstTouch) {
        // this is the first touch point
        if (m_secondTouch.isValid()) {
            // make the second touch the first touch:
            m_firstTouch = m_secondTouch;
            m_secondTouch.invalidate();
            emit touchUp(&touch, false, canceled);
        } else {
            // this is the only touch point
            m_firstTouch.invalidate();
            setPressed(false);
            if (touch.isAtOrigin() && !canceled) {
                emit click(&touch);
                double sinceLastClick = HighResTime::getElapsedSecAndUpdate(m_lastClickTimestamp);
                if (sinceLastClick * 1000 < TouchAreaConstants::doubleClickDuration) {
                    emit doubleClick(&touch);
                }
                if (touch.isRightButton()) {
                    emit rightClick(&touch);
                } else if (m_longClickTimer.isActive()) {
                    m_longClickTimer.stop();
                    emit shortClick(&touch);
                }
            }
            emit touchUp(&touch, false, canceled);
        }
    } else if (touch == m_secondTouch) {
        // this is the second touch point
        m_secondTouch.invalidate();
        emit touchUp(&touch, true, canceled);
    } else {
        // this is an unknown touch point
        // ignore it:
        touch.setAccepted(false);
    }
}

// ---------------- Wheel + Click + Hover ---------------------

void TouchArea::mouseDoubleClickEvent(QMouseEvent* event) {
    TouchAreaEvent touch(event, this);
    m_mouseLast = touch;
    emit doubleClick(&m_mouseLast);
}

void TouchArea::wheelEvent(QWheelEvent* event) {
    m_scrollEventAccepted = false;
    if (!event->pixelDelta().isNull()) {
        QPoint px = event->pixelDelta();
        emit scrollEvent(px.x(), px.y());
    } else {
        QPoint dg = event->angleDelta();
        // one "step" is 120 units or 15 degree
        dg *= (TouchAreaConstants::pixelPerWheelStep / 120.0);
        emit scrollEvent(dg.x(), dg.y());
    }
    event->setAccepted(m_scrollEventAccepted);
}

void TouchArea::hoverEnterEvent(QHoverEvent* /*event*/) {
    m_mouseOver = true;
    emit mouseOverChanged();

}

void TouchArea::hoverMoveEvent(QHoverEvent* /*event*/) {

}

void TouchArea::hoverLeaveEvent(QHoverEvent* /*event*/) {
    m_mouseOver = false;
    emit mouseOverChanged();
}
