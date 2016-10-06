import QtQuick 2.5
import CustomElements 1.0

CustomTouchArea {
    width: 20*dp
    height: 30*dp
    anchors.left: parent.left

    property Item target
    property int minSize: 60*dp
    property int maxSize: 200*dp
    property int initialWidth: 0
    property int initialBottom: 0
    property int initialRight: 0
    property int initialTouchX: 0

    onTouchDown: {
        initialWidth = target.width
        initialBottom = target.y + target.height
        initialRight = target.x + target.width
    }

    onTouchMove: {
        target.width = Math.max(minSize, Math.min(touch.originX - touch.x + initialWidth, maxSize))
        target.y = initialBottom - target.height
        target.x = initialRight - target.width
        target.block.positionChangedExternal()
    }

    Rectangle {
        width: 1*dp
        height: 20*dp
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.horizontalCenterOffset: -3*dp
        color: "#888"
    }

    Rectangle {
        width: 1*dp
        height: 20*dp
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.horizontalCenterOffset: 3*dp
        color: "#888"
    }
}
