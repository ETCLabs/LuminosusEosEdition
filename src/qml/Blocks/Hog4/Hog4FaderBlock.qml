import QtQuick 2.5
import CustomElements 1.0
import "../../CustomBasics"
import "../../CustomControls"

BlockBase {
    id: root
    width: 90*dp
    height: 400*dp
    settingsComponent: settings

    StretchColumn {
        anchors.fill: parent

        PushButton {
            implicitHeight: 0  // do not stretch
            height: 60*dp
            pressed: block.attr("chooseButton").val
            onActiveChanged: {
                if (block.attr("chooseButton").val !== active) {
                    block.attr("chooseButton").val = active
                }
            }
            mappingID: block.getUid() + "choose"
            text: block.attr("masterNumber").val
            marked: block.attr("chooseLed").val
            InputNode {
                anchors.verticalCenter: parent.verticalCenter
                node: block.node("choose")
            }
        }

        Item {
            height: 10*dp
        }

        PushButton {
            implicitHeight: 0  // do not stretch
            height: 40*dp
            pressed: block.attr("goButton").val
            onActiveChanged: {
                if (block.attr("goButton").val !== active) {
                    block.attr("goButton").val = active
                }
            }
            showLed: false
            mappingID: block.getUid() + "go"
            Image {
                width: 15*dp
                height: 15*dp
                anchors.centerIn: parent
                anchors.verticalCenterOffset: parent.pressed ? -2*dp : -4*dp
                source: "qrc:/images/play_icon.png"
            }

            InputNode {
                anchors.verticalCenter: parent.verticalCenter
                node: block.node("go")
            }
        }

        PushButton {
            implicitHeight: 0  // do not stretch
            height: 40*dp
            pressed: block.attr("pauseButton").val
            onActiveChanged: {
                if (block.attr("pauseButton").val !== active) {
                    block.attr("pauseButton").val = active
                }
            }
            showLed: false
            mappingID: block.getUid() + "pause"
            Image {
                width: 15*dp
                height: 15*dp
                anchors.centerIn: parent
                anchors.verticalCenterOffset: parent.pressed ? -2*dp : -4*dp
                source: "qrc:/images/pause_icon.png"
            }

            InputNode {
                anchors.verticalCenter: parent.verticalCenter
                node: block.node("pause")
            }
        }

        PushButton {
            implicitHeight: 0  // do not stretch
            height: 40*dp
            pressed: block.attr("backButton").val
            onActiveChanged: {
                if (block.attr("backButton").val !== active) {
                    block.attr("backButton").val = active
                }
            }
            showLed: false
            mappingID: block.getUid() + "back"
            Image {
                width: 15*dp
                height: 15*dp
                anchors.centerIn: parent
                anchors.verticalCenterOffset: parent.pressed ? -2*dp : -4*dp
                source: "qrc:/images/play_icon.png"
                rotation: 180
            }

            InputNode {
                anchors.verticalCenter: parent.verticalCenter
                node: block.node("back")
            }
        }

        AttributeSlider {
            implicitHeight: -1
            attr: block.attr("faderValue")

            InputNode {
                anchors.verticalCenter: parent.verticalCenter
                node: block.node("inputNode")
            }
        }

        PushButton {
            implicitHeight: 0  // do not stretch
            height: 50*dp
            pressed: block.attr("flashButton").val
            marked: block.attr("flashLed").val
            onActiveChanged: {
                if (block.attr("flashButton").val !== active) {
                    block.attr("flashButton").val = active
                }
            }
            mappingID: block.getUid() + "flash"

            InputNode {
                anchors.verticalCenter: parent.verticalCenter
                node: block.node("flash")
            }
        }

        BlockRow {
            height: 30*dp
            implicitHeight: 0  // do not stretch
            StretchText {
                text: "#:"
            }
            AttributeNumericInput {
                width: 40*dp
                attr: block.attr("masterNumber")
            }
        }

        DragArea {
        }

    }  // end main Column

    Component {
        id: settings
        StretchColumn {
            leftMargin: 15*dp
            rightMargin: 15*dp
            defaultSize: 30*dp

            BlockRow {
                Text {
                    text: "Master Number:"
                    width: parent.width - 40*dp
                }
                AttributeNumericInput {
                    width: 40*dp
                    attr: block.attr("masterNumber")
                }
            }
        }
    }  // end Settings Component
}
