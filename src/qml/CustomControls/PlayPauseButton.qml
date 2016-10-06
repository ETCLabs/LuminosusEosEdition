import QtQuick 2.5
import "../CustomBasics"
import "../."

Button {

    property bool isPlaying: false
    property string iconName: block.isPlaying ? "pause_icon": "play_icon"

    // ------------------------ Icon Image -------------------------

    Image {
        id: icon
        source: "qrc:/images/" + iconName + ".png"
        width: Math.min(parent.height, parent.width) - 16*dp
        height: width
        anchors.centerIn: parent
        smooth: true
    }

    // ------------------- Touch Feedback Effect ------------------------------

    RippleEffect {
        id: rippleEffect
        anchors.fill: parent
    }
    onTouchDown: rippleEffect.touchStart(touch.itemX, touch.itemY)
    onTouchUp: rippleEffect.touchEnd()
}
