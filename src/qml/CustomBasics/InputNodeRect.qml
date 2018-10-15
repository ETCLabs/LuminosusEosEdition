import QtQuick 2.5
import CustomElements 1.0

Item {
    property QtObject node
    property var suggestions: []
    width: 15*dp
    height: 30*dp
    anchors.left: parent.left

    Component.onCompleted: {
        node.setGuiItem(this)
    }

    Image {
        width: 30*dp
        height: 30*dp
        x: -15*dp
        source: dp <= 1 ? (node.focused ? "qrc:/images/node_rect_red.png" : "qrc:/images/node_rect_" + (node.htpMode ? "cyan.png" : "blue.png"))
                        : (node.focused ? "qrc:/images/node_rect_red@2x.png" : "qrc:/images/node_rect_" + (node.htpMode ? "cyan@2x.png" : "blue@2x.png"))
    }

    CustomTouchArea {
        id: touchArea
        width: 30*dp
        height: 30*dp
        x: -15*dp

        clickDurationEnabled: true
        onShortClick: {
            controller.playClickSound()
            node.onTouched()
        }
        onLongClick: {
            controller.playClickUpSound()
            node.toggleHtpMode()
            if (node.htpMode) {
                guiManager.showToast("HTP Mode")
            } else {
                guiManager.showToast("LTP Mode")
            }
        }
    }

    Connections {
        target: node
        onFocusedChanged: if (node.focused) suggestionsLoader.active = true
    }

    Loader {
        id: suggestionsLoader
        active: false
        width: 1
        height: implicitHeight
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.left
        anchors.rightMargin: 30*dp

        sourceComponent: Column {
            id: suggestionsColumn
            spacing: 10*dp

            visible: opacity > 0
            opacity: 0.0
            Behavior on opacity {
                NumberAnimation {
                    duration: 150
                }
            }
            Component.onCompleted: opacity = (node.focused && !node.isConnected) ? 1.0 : 0.0
            Connections {
                target: node
                onFocusedChanged: suggestionsColumn.opacity = (node.focused && !node.isConnected) ? 1.0 : 0.0
            }

            Repeater {
                model: suggestions

                CustomTouchArea {
                    id: suggestionTouchArea
                    visible: controller.blockManager().blockList().blockExists(modelData)
                    height: 30*dp
                    width: suggestionLabel.contentWidth + 10*dp
                    x: -width
                    mouseOverEnabled: true
                    onClick: controller.blockManager().addBlockByNameQml(modelData)

                    Rectangle {
                        anchors.fill: parent
                        radius: 4*dp
                        antialiasing: false
                        color: "#555"
                        border.color: "#fff"
                        border.width: 1
                        opacity: suggestionTouchArea.mouseOver ? 1.0 : 0.4

                        Text {
                            id: suggestionLabel
                            anchors.centerIn: parent
                            text: controller.blockManager().blockList().getNameInUi(modelData) + " +"
                            font.pixelSize: 15*dp
                            color: "white"
                        }
                    }
                }  // end Repeater delegate
            }
        }
    }  // Loader
}

