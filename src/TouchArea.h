#ifndef TOUCHAREA_H
#define TOUCHAREA_H

#include "utils.h"

#include <QQuickItem>
#include <QPointer>
#include <QTimer>


// forward declaration
class TouchArea;

/**
 * @brief The TouchAreaConstants namespace contains all constants used in TouchArea and TouchAreaEvent.
 */
namespace TouchAreaConstants {

    /**
     * @brief defaultLongClickDuration min. duration of a long click
     */
    static const int defaultLongClickDuration = 800;  // ms

    /**
     * @brief pixelPerWheelStep pixel to scroll per mouse wheel step
     */
    static const int pixelPerWheelStep = 40;

    /**
     * @brief mouseEventId special event id for mouse events to differentiate it from touch events
     */
    static const int mouseEventId = 9999;

    /**
     * @brief defaultIsAtOriginTolerance tolerance in pixel for "is at origin" property
     */
    static const double defaultIsAtOriginTolerance = 5;

    /**
     * @brief doubleClickDuration max. double click duration
     */
    static const double doubleClickDuration = 400;  // ms
}


/**
 * @brief The TouchAreaEvent class represents an input event in a TouchArea item.
 */
class TouchAreaEvent : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool isValid READ isValid NOTIFY isValidChanged)
    Q_PROPERTY(qreal x MEMBER m_x NOTIFY dataChanged)
    Q_PROPERTY(qreal y MEMBER m_y NOTIFY dataChanged)
    Q_PROPERTY(qreal originX MEMBER m_originX NOTIFY dataChanged)
    Q_PROPERTY(qreal originY MEMBER m_originY NOTIFY dataChanged)
    Q_PROPERTY(qreal itemX MEMBER m_itemX NOTIFY dataChanged)
    Q_PROPERTY(qreal itemY MEMBER m_itemY NOTIFY dataChanged)
    Q_PROPERTY(qreal itemOriginX MEMBER m_itemOriginX NOTIFY dataChanged)
    Q_PROPERTY(qreal itemOriginY MEMBER m_itemOriginY NOTIFY dataChanged)
    Q_PROPERTY(qreal sceneX MEMBER m_sceneX NOTIFY dataChanged)
    Q_PROPERTY(qreal sceneY MEMBER m_sceneY NOTIFY dataChanged)
    Q_PROPERTY(qreal deltaX MEMBER m_deltaX NOTIFY dataChanged)
    Q_PROPERTY(qreal deltaY MEMBER m_deltaY NOTIFY dataChanged)
    Q_PROPERTY(bool isTouch MEMBER m_isTouch NOTIFY dataChanged)
    Q_PROPERTY(bool isRightButton MEMBER m_isRightButton NOTIFY dataChanged)
    Q_PROPERTY(Qt::KeyboardModifiers modifiers MEMBER m_modifiers NOTIFY dataChanged)

    Q_PROPERTY(bool accepted READ isAccepted WRITE setAccepted NOTIFY acceptedChanged)

public:
    /**
     * @brief TouchAreaEvent creates an empty dummy event.
     */
    TouchAreaEvent();
    /**
     * @brief TouchAreaEvent creates an empty dummy event.
     * @param touchArea origin TouchArea
     */
    TouchAreaEvent(TouchArea* touchArea);
    /**
     * @brief TouchAreaEvent creates a copy of a TouchAreaEvent
     * @param other TouchAreaEvent to copy
     */
    TouchAreaEvent(const TouchAreaEvent& other);
    /**
     * @brief TouchAreaEvent creates event from touch input
     * @param touchPoint the touch point
     * @param modifiers keyboard modifiers
     * @param touchArea origin TouchArea
     */
    TouchAreaEvent(const QTouchEvent::TouchPoint& touchPoint, Qt::KeyboardModifiers modifiers, TouchArea* touchArea);
    /**
     * @brief TouchAreaEvent creates event from mouse input without prior mouse event
     * @param mouseEvent the mouse event
     * @param touchArea origin TouchArea
     */
    TouchAreaEvent(const QMouseEvent* mouseEvent, TouchArea* touchArea);
    /**
     * @brief TouchAreaEvent creates event from mouse input with prior mouse event
     * @param mouseEvent the mouse event
     * @param mouseLast the previous mouse event
     * @param touchArea origin touch area
     */
    TouchAreaEvent(const QMouseEvent* mouseEvent, const TouchAreaEvent& mouseLast, TouchArea* touchArea);

    const TouchAreaEvent& operator=(const TouchAreaEvent& other);
    bool operator==(const TouchAreaEvent& other) const { return m_id == other.m_id; }
    bool equals(const TouchAreaEvent* other) const { return m_id == other->m_id; }

signals:
    /**
     * @brief isValidChanged emitted when "is valid" attribut changed
     */
    void isValidChanged();
    /**
     * @brief dataChanged emitted when internal data changed
     */
    void dataChanged();
    /**
     * @brief acceptedChanged emitted when accapted state changed
     */
    void acceptedChanged();

public slots:
    /**
     * @brief isValid returns if this event is in a valid state
     * @return true if it is valid, if not it should not be used
     */
    bool isValid() const { return m_isValid; }
    /**
     * @brief invalidate invalidates the event
     */
    void invalidate();

    /**
     * @brief isAccepted returns if the event was accepted
     * @return true if it was accepted
     */
    bool isAccepted() const { return m_accepted; }
    /**
     * @brief setAccepted sets if the event was accepted
     * @param value true if accepted
     */
    void setAccepted(bool value) { m_accepted = value; emit acceptedChanged(); }

    /**
     * @brief isAtOrigin returns if the touch position hasn't moved
     * @param tolerance in pixel
     * @return true if it hasn't moved (is a tap or click)
     */
    bool isAtOrigin(qreal tolerance = TouchAreaConstants::defaultIsAtOriginTolerance) const;

    /**
     * @brief isTouch returns if this event comes from a touch input device
     * @return true if it is a touch, false if it is mouse input
     */
    bool isTouch() const { return m_isTouch; }
    /**
     * @brief isRightButton returns if the event comes from a right click
     * @return true if it is a right click
     */
    bool isRightButton() const { return m_isRightButton; }

protected:
    bool m_isValid;  //!< true if event is valid, if false it should not be used

    qreal m_x;  //!< current position in screen coordinates
    qreal m_y;  //!< current position in screen coordinates
    qreal m_originX;  //!< origin position in screen coordinates
    qreal m_originY;  //!< origin position in screen coordinates

    qreal m_itemX;  //!< current position in item coordinates
    qreal m_itemY;  //!< current position in item coordinates
    qreal m_itemOriginX;  //!< origin position in item coordinates
    qreal m_itemOriginY;  //!< origin position in item coordinates

    qreal m_sceneX;  //!< current position in scene / window coordinates
    qreal m_sceneY;  //!< current position in scene / window coordinates

    qreal m_deltaX;  //!< amount of pixel moved since last event
    qreal m_deltaY;  //!< amount of pixel moved since last event

    bool m_isTouch;  //!< true if touch, false if mouse input
    bool m_isRightButton;  //!< true if it was a right click
    Qt::KeyboardModifiers m_modifiers;  //!< keyboard modifiers at event time
    int m_id;  //!< id of the event (only unique at the time of the event, number of touch on screen)

    bool m_accepted;  //!< true if event was accepted
};

Q_DECLARE_METATYPE(TouchAreaEvent)

// -------------------------------------------------------------------------

/**
 * @brief The TouchArea class is a QML item that handles mouse and touch input.
 */
class TouchArea : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(bool pressed READ getPressed NOTIFY pressedChanged)
    Q_PROPERTY(bool mouseOverEnabled READ getMouseOverEnabled WRITE setMouseOverEnabled NOTIFY mouseOverEnabledChanged)
    Q_PROPERTY(bool mouseOver MEMBER m_mouseOver NOTIFY mouseOverChanged)
    Q_PROPERTY(bool clickDurationEnabled READ getClickDurationEnabled WRITE setClickDurationEnabled NOTIFY clickDurationEnabledChanged)
    Q_PROPERTY(int longClickDuration READ getLongClickDuration WRITE setLongClickDuration NOTIFY longClickDurationChanged)
    Q_PROPERTY(bool secondTouchEnabled READ getSecondTouchEnabled WRITE setSecondTouchEnabled NOTIFY secondTouchEnabledChanged)


public:
    /**
     * @brief TouchArea creates a TouchArea item
     * @param parent QQuickItem parent object
     */
    TouchArea(QQuickItem* parent = 0);

signals:
    /**
     * @brief touchDown is emitted when a mouse is pressed or a touch happens in this area
     * @param touch a pointer to the corresponding TouchAreaEvent
     * @param isSecond true if this is the second touch in this area
     */
    void touchDown(TouchAreaEvent* touch, bool isSecond);
    /**
     * @brief touchMove is emitted when a mouse is pressed and moved or a touch moves thats origin is in this area
     * @param touch a pointer to the corresponding TouchAreaEvent
     * @param isSecond true if this is the second touch in this area
     */
    void touchMove(TouchAreaEvent* touch, bool isSecond);
    /**
     * @brief touchUp is emitted when a mouse button or touch thats origin is in this area
     * @param touch a pointer to the corresponding TouchAreaEvent
     * @param isSecond true if this is the second touch in this area
     * @param canceled true if this event was canceled because another item handles it now (i.e. ScrollView)
     */
    void touchUp(TouchAreaEvent* touch, bool isSecond, bool canceled);
    /**
     * @brief touchCanceled is emitted when this event was canceled because another item handles it now (i.e. ScrollView)
     */
    void touchCanceled();

    /**
     * @brief click is emitted when a click or tap is recognized (-> it didn't move until release)
     * @param touch a pointer to the corresponding TouchAreaEvent
     */
    void click(TouchAreaEvent* touch);
    /**
     * @brief shortClick is emitted when a short click or tap is recognized (-> it didn't move until release)
     * @param touch a pointer to the corresponding TouchAreaEvent
     */
    void shortClick(TouchAreaEvent* touch);
    /**
     * @brief longClick is emitted when a long click or tap is recognized (-> it didn't move until release)
     * @param touch a pointer to the corresponding TouchAreaEvent
     */
    void longClick(TouchAreaEvent* touch);
    /**
     * @brief rightClick is emitted when a right click is recognized (-> it didn't move until release)
     * @param touch a pointer to the corresponding TouchAreaEvent
     */
    void rightClick(TouchAreaEvent* touch);
    /**
     * @brief doubleClick is emitted when a double click or tap is recognized (-> it didn't move until release)
     * @param touch a pointer to the corresponding TouchAreaEvent
     */
    void doubleClick(TouchAreaEvent* touch);

    /**
     * @brief scrollEvent is emitted when the scroll wheel was moved or a scroll gesture performed
     * @param deltaX in pixel
     * @param deltaY in pixel
     */
    void scrollEvent(int deltaX, int deltaY);

    /**
     * @brief pressedChanged is emitted when the pressed state changed
     */
    void pressedChanged();
    /**
     * @brief mouseOverEnabledChanged is emitted when the mouse over setting changed
     */
    void mouseOverEnabledChanged();
    /**
     * @brief mouseOverChanged is emitted when the "mouse over" state changed, i.e. mouse entered
     * or left the area
     */
    void mouseOverChanged();
    /**
     * @brief clickDurationEnabledChanged is emitted when the "click duration enabled" setting changed
     */
    void clickDurationEnabledChanged();
    /**
     * @brief longClickDurationChanged is emitted when the "long click duration" setting changed
     */
    void longClickDurationChanged();
    /**
     * @brief secondTouchEnabledChanged is emitted when the "second touch enabled" settings changed
     */
    void secondTouchEnabledChanged();

public slots:
    /**
     * @brief getFirstTouch returns the first touch or click
     * @return pointer to first touch event (is invalid if not existent)
     */
    TouchAreaEvent* getFirstTouch() { return &m_firstTouch; }
    /**
     * @brief getSecondTouch returns the second touch or click
     * @return pointer to second touch event or nullptr if not existent
     */
    TouchAreaEvent* getSecondTouch() { return &m_secondTouch; }

    /**
     * @brief getPressed returns if there is a touch or click in this area
     * @return true if it is "pressed"
     */
    bool getPressed() const { return m_pressed; }
    /**
     * @brief setPressed internal method to set "pressed" property
     * @param value true if it is pressed
     */
    void setPressed(bool value) { m_pressed = value; emit pressedChanged(); }

    /**
     * @brief getMouseOverEnabled returns if the mouse over detection is enabled
     * @return true if mouse over is enabled
     */
    bool getMouseOverEnabled() const { return acceptHoverEvents(); }
    /**
     * @brief setMouseOverEnabled enable or disable mouse over detection
     * @param value true to enable it
     */
    void setMouseOverEnabled(bool value) { setAcceptHoverEvents(value); emit mouseOverEnabledChanged(); }

    /**
     * @brief getClickDurationEnabled return if the clik duration detection is enabled
     * @return true if it is enabled
     */
    bool getClickDurationEnabled() const { return m_clickDurationEnabled; }
    /**
     * @brief setClickDurationEnabled enables or disables clik duration detection
     * @param value true to enable it
     */
    void setClickDurationEnabled(bool value) { m_clickDurationEnabled = value; emit clickDurationEnabledChanged(); }

    /**
     * @brief getLongClickDuration returns the minimum duration of a long click
     * @return min. duration of long click in ms
     */
    int getLongClickDuration() const { return m_longClickDuration; }
    /**
     * @brief setLongClickDuration sets the minimum duration of a long click
     * @param value min. duration of long click in ms
     */
    void setLongClickDuration(int value) { m_longClickDuration = value; emit longClickDurationChanged(); }

    /**
     * @brief getSecondTouchEnabled returns if this area supports a second touch or click
     * @return true if second touch is enabled
     */
    bool getSecondTouchEnabled() const { return m_secondTouchEnabled; }
    /**
     * @brief setSecondTouchEnabled enables or disables if this area supports a second touch or click
     * @param value true to enable second touch
     */
    void setSecondTouchEnabled(bool value) { m_secondTouchEnabled = value; emit secondTouchEnabledChanged(); }

    /**
     * @brief scrollEventWasAccepted tells this object that the last scroll event was handled
     */
    void scrollEventWasAccepted() { m_scrollEventAccepted = true; }

private slots:
    /**
     * @brief onLongClickTimeout checks if there was a long click after the long click duration
     */
    void onLongClickTimeout();

protected:
    // --------------- Mouse and Touch movement ---------------------

    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseUngrabEvent() override;

    void touchEvent(QTouchEvent* event) override;
    void touchUngrabEvent() override;

    /**
     * @brief onTouchDownInternal combined event handler for both touch and click start
     * @param touch the touch or click event
     */
    void onTouchDownInternal(TouchAreaEvent& touch);
    /**
     * @brief onTouchMoveInternal combined event handler for both touch and click movement
     * @param touch the touch or click event
     */
    void onTouchMoveInternal(TouchAreaEvent& touch);
    /**
     * @brief onTouchUpInternal combined event handler for both touch and click movement
     * @param touch the touch or click event
     * @param canceled true if the event was canceled (i.e. because of scrolling)
     */
    void onTouchUpInternal(TouchAreaEvent& touch, bool canceled = false);

    // ---------------- Wheel + Click + Hover ---------------------

    void mouseDoubleClickEvent(QMouseEvent* event) override;

    void wheelEvent(QWheelEvent* event) override;

    void hoverEnterEvent(QHoverEvent* /*event*/) override;
    void hoverMoveEvent(QHoverEvent* /*event*/) override;
    void hoverLeaveEvent(QHoverEvent* /*event*/) override;

    // ---------------------

    bool m_pressed;  //!< true if there is at least one touch or click in this area

    bool m_mouseOver;  //!< true if mouse over is enabled and mouse is in this area

    bool m_clickDurationEnabled;  //!< true if click duration detection is enabled
    int m_longClickDuration;  //!< min. duration of long click in ms
    QTimer m_longClickTimer;  //!< Timer to detect long click event

    bool m_secondTouchEnabled;  //!< true if this area supports a second touch or click
    TouchAreaEvent m_firstTouch;  //!< first touch or click event (is invalid if not existent)
    TouchAreaEvent m_secondTouch;  //!< second touch or click event (is invalid if not existent)

    TouchAreaEvent m_mouseLast;  //!< the last mouse event (used to calculate deltas)

    HighResTime::time_point_t m_lastClickTimestamp;  //!< time of last click to detect double click

    bool m_scrollEventAccepted;

};

#endif // TOUCHAREA_H
