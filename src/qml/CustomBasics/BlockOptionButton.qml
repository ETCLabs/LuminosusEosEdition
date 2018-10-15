import QtQuick 2.5
import QtGraphicalEffects 1.0
import CustomElements 1.0

Rectangle {
    id: root
    visible: anchors.topMargin > -height
    width: 25*dp
    height: 30*dp
    anchors.top: parent.bottom
    anchors.topMargin: block.focused && enabled ? (settingsButtonTouchArea.mouseOver || active ? -5*dp : -10*dp) : -height
    radius: 5*dp
    antialiasing: dp < 2
    color: "#444"
    border.color: "#ccc"
    border.width: 1
    opacity: settingsButtonTouchArea.mouseOver || active ? 1.0 : 0.6

    property bool active: false
    property string text: ""
    property string icon: ""

    property Component content: Item {
        width: 200*dp
        height: 20*dp
    }

    Behavior on anchors.topMargin {
        NumberAnimation {
            duration: 400
            easing.type: Easing.OutCubic
        }
    }

    Connections {
        target: block
        onFocusedChanged: if (!block.focused) active = false
    }

    CustomTouchArea {
        id: settingsButtonTouchArea
        anchors.fill: parent
        mouseOverEnabled: true
        onClick: root.active = !root.active
    }

    Text {
        visible: root.text || root.active
        anchors.centerIn: parent
        anchors.verticalCenterOffset: 3*dp
        color: "white"
        font.pixelSize: 11*dp
        text: active ? "X" : root.text
    }

    Image {
        visible: root.icon && !active
        anchors.centerIn: parent
        anchors.verticalCenterOffset: 3*dp
        source: root.icon ? "qrc:/images/" + root.icon + ".png" : ""
        width: 10*dp
        height: width
        smooth: true
    }

    Rectangle {
        visible: opacity > 0
        width: contentLoader.width + 10*dp
        height: contentLoader.height + 10*dp
        anchors.top: parent.bottom
        anchors.topMargin: 5*dp
        radius: 5*dp
        antialiasing: dp < 2
        color: "#444"
        border.color: "#ccc"
        border.width: 1

        opacity: root.active ? 1.0 : 0.0
        Behavior on opacity {
            NumberAnimation {
                duration: 300
                easing.type: Easing.OutCubic
            }
        }

        Loader {
            id: contentLoader
            anchors.centerIn: parent
            width: item ? item.width : 270*dp
            active: root.active
            sourceComponent: root.content
        }
    }
}
