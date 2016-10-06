import QtQuick 2.5
import CustomElements 1.0
import "../CustomBasics"
import "../CustomControls"
import "../."  // to import EosFaderItem

BlockBase {
    id: root
    width: 120*dp
    height: 400*dp
    settingsComponent: settings

    StretchColumn {
        anchors.fill: parent

        Text {
            height: 40*dp
            text: block.faderLabel
            horizontalAlignment: Text.AlignHCenter
            fontSizeMode: Text.Fit
            color: (text[0] === "S") ? "lightgreen" : "white"
        }
        PushButton {
            implicitHeight: 0  // do not stretch
            height: 40*dp
            text: block.faderNumber
            onTouchDown: {
                if (touch.modifiers & Qt.ShiftModifier) {
                    block.sendUnloadEvent()
                } else {
                    block.sendLoadEvent(true)
                }
            }
            onTouchUp: {
                if (!(touch.modifiers & Qt.ShiftModifier)) {
                    block.sendLoadEvent(false)
                }
            }
            showLed: false
        }
        Slider {
            implicitHeight: -1
            value: block.faderLevel
            onValueChanged: {
                if (value !== block.faderLevel) {
                    block.setFaderLevelFromGui(value)
                }
            }
        }
        PushButton {
            implicitHeight: 0  // do not stretch
            height: 40*dp
            onActiveChanged: block.sendStopEvent(active)
            showLed: false
            Image {
                width: 12*dp
                height: 12*dp
                anchors.centerIn: parent
                anchors.verticalCenterOffset: parent.pressed ? -2*dp : -4*dp
                source: "qrc:/images/fader_stop_icon.png"
            }

            InputNode {
                objectName: "stopNode"
            }
        }
        PushButton {
            implicitHeight: 0  // do not stretch
            height: 40*dp
            onActiveChanged: block.sendFireEvent(active)
            showLed: false
            Image {
                width: 15*dp
                height: 15*dp
                anchors.centerIn: parent
                anchors.verticalCenterOffset: parent.pressed ? -2*dp : -4*dp
                source: "qrc:/images/fader_go_icon.png"
            }

            InputNode {
                objectName: "fireNode"
            }
        }

        BlockRow {
            height: 30*dp
            implicitHeight: 0  // do not stretch
            StretchText {
                text: "Number:"
            }
            NumericInput {
                width: 40*dp
                implicitWidth: 0  // do not stretch
                minimumValue: 1
                maximumValue: 10
                value: block.faderNumber
                onValueChanged: {
                    if (value !== block.faderNumber) {
                        block.faderNumber = value
                    }
                }
            }
        }

        BlockRow {
            height: 30*dp
            implicitHeight: 0  // do not stretch
            StretchText {
                width: 50*dp
                text: "Page:"
            }
            NumericInput {
                width: 40*dp
                implicitWidth: 0  // do not stretch
                minimumValue: 1
                maximumValue: 100
                value: block.page
                onValueChanged: {
                    if (value !== block.page) {
                        block.page = value
                    }
                }
            }
        }

        DragArea {
            text: "Fader"

            InputNode {
                objectName: "inputNode"
            }
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
                    text: "Fader Page:"
                    width: parent.width - 40*dp
                }
                NumericInput {
                    width: 40*dp
                    minimumValue: 1
                    maximumValue: 100
                    value: block.page
                    onValueChanged: {
                        if (value !== block.page) {
                            block.page = value
                        }
                    }
                }
            }
            BlockRow {
                Text {
                    text: "Fader Number:"
                    width: parent.width - 40*dp
                }
                NumericInput {
                    width: 40*dp
                    minimumValue: 1
                    maximumValue: 10
                    value: block.faderNumber
                    onValueChanged: {
                        if (value !== block.faderNumber) {
                            block.faderNumber = value
                        }
                    }
                }
            }
        }
    }  // end Settings Component
}
