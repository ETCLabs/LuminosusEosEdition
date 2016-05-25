import QtQuick 2.0
import "../CustomBasics"
import "../."

Button {
    Rectangle {
        color: Qt.rgba(0.3, 0.5, 1, 0.7)
        height: 1*dp
        width: parent.width - 16*dp
        x: 8*dp
        y: parent.height - 6*dp
    }

    Rectangle {
        property real ratio: parent.active ? 1 : 0
        color: "yellow"
        height: 1*dp
        width: (parent.width - 16*dp) * ratio
        x: 8*dp + (1 - ratio) * (parent.width / 2 - 8*dp)
        y: parent.height - 6*dp
        Behavior on ratio {
           NumberAnimation { duration: 100 }
       }
    }
    RippleEffect {
        id: rippleEffect
		anchors.fill: parent
		visible: GRAPHICAL_EFFECTS_LEVEL >= 3
    }
    onTouchDown: rippleEffect.touchStart(touch.x, touch.y)
    onTouchUp: rippleEffect.touchEnd()
}
