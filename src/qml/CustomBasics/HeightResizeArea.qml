import QtQuick 2.5
import CustomElements 1.0

CustomTouchArea {
    height: 20*dp

    // public
    property Item target
    property int minSize: 120*dp
    property int maxSize: 600*dp

    // ----------------------- Logic ----------------------

    // internal
    property int initialHeight: 0
    property int initialBottom: 0

    onTouchDown: {
        initialHeight = target.height
        initialBottom = target.y + target.height
    }

    onTouchMove: {
        target.height = Math.max(minSize, Math.min(touch.originY - touch.y + initialHeight, maxSize))
        target.y = initialBottom - target.height
        target.block.positionChangedExternal()
    }

    // ----------------------- Visuals ----------------------

    Rectangle {
        height: 1*dp
        width: 20*dp
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        anchors.verticalCenterOffset: -3*dp
        color: "#888"
    }

    Rectangle {
        height: 1*dp
        width: 20*dp
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        anchors.verticalCenterOffset: 3*dp
        color: "#888"
    }
}
