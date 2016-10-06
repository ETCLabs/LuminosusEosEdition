import QtQuick 2.5
import CustomElements 1.0

CustomTouchArea {
    width: 20*dp
    height: 30*dp
    anchors.right: parent.right

    // public
    property Item target
    property int minSize: 50*dp
    property int maxSize: 200*dp

    // ----------------------- Logic ----------------------

    // internal
    property int initialWidth: 0

    onTouchDown: {
        initialWidth = target.width
    }

    onTouchMove: {
        target.width = Math.max(minSize, Math.min(touch.x - touch.originX + initialWidth, maxSize))
        target.block.positionChangedExternal()
    }

    // ----------------------- Visuals ----------------------

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
