import QtQuick 2.5
import CustomElements 1.0
import "../../CustomBasics"
import "../../CustomControls"

BlockBase {
    id: root
    width: 180*dp
    height: 8*30*dp + 1
    settingsComponent: settings

    StretchColumn {
        anchors.fill: parent

        BlockRow {
            Item {
                width: 20*dp
                Rectangle {
                    anchors.centerIn: parent
                    width: 7*dp
                    height: width
                    radius: width / 2
                    antialiasing: dp <= 1
                    color: block.attr("connected").val ? (block.attr("authenticated").val ? "green" : "orange") : "red"
                }
            }
            StretchText {
                text: block.attr("authenticated").val ? block.attr("projectorName").val || "Unkown" : (block.attr("passwordIsWrong").val ? "Wrong Password" : "Not Connected")
                elide: Text.ElideRight
            }
        }

        Rectangle {
            height: 1
            color: "#888"
        }

        BlockRow {
            leftMargin: 10*dp
            rightMargin: 10*dp
            StretchText {
                text: "Power:"
            }
            StretchText {
                hAlign: Text.AlignHCenter
                text: {
                    var status = block.attr("powerStatus").val
                    if (status === 0) {
                        return "Off"
                    } else if (status === 1) {
                        return "On"
                    } else if (status === 2) {
                        return "Cooling"
                    } else if (status === 3) {
                        return "Warm-Up"
                    } else {
                        return "Unkown"
                    }
                }
                color: {
                    var status = block.attr("powerStatus").val
                    if (status === 0) {
                        return "red"
                    } else if (status === 1) {
                        return "green"
                    } else if (status === 2) {
                        return "orange"
                    } else if (status === 3) {
                        return "orange"
                    } else {
                        return "red"
                    }
                }
                font.bold: true
            }
        }

        BlockRow {
            InputNode {
                node: block.node("on")
                suggestions: ["Eos Cue", "OSC In"]
            }
            ButtonBottomLine {
                text: "Turn On"
                clickDurationEnabled: true
                onShortClick: controller.guiManager().showToast("Hold button to turn on")
                onLongClick: block.turnOn()
            }
        }

        BlockRow {
            InputNode {
                node: block.node("off")
                suggestions: ["Eos Cue", "OSC In"]
            }
            ButtonBottomLine {
                text: "Turn Off"
                clickDurationEnabled: true
                onShortClick: controller.guiManager().showToast("Hold button to turn off")
                onLongClick: block.turnOff()
            }
        }

        BlockRow {
            leftMargin: 10*dp
            rightMargin: 10*dp
            StretchText {
                text: "Shutter:"
            }
            StretchText {
                hAlign: Text.AlignHCenter
                text: block.attr("shutterIsOpen").val ? "Open" : "Closed"
                color: block.attr("shutterIsOpen").val ? "green" : "red"
                font.bold: true
            }
        }

        BlockRow {
            InputNode {
                node: block.node("openShutter")
                suggestions: ["Eos Cue", "OSC In"]
            }
            ButtonBottomLine {
                text: "Open Shutter"
                onPress: block.turnAvMuteOff()
            }
        }

        BlockRow {
            InputNode {
                node: block.node("inputNode")
                suggestions: ["Eos Cue", "OSC In"]
            }
            ButtonBottomLine {
                text: "Close Shutter"
                onPress: block.turnAvMuteOn()
            }
        }

        DragArea {
            text: "Projector"
        }
    }  // end main column



    Component {
        id: settings
        StretchColumn {
            leftMargin: 15*dp
            rightMargin: 15*dp
            defaultSize: 30*dp

            BlockRow {
                Text {
                    text: "IP Address:"
                    width: parent.width / 2
                }
                TextInput {
                    text: block.attr("ipAddress").val
                    width: parent.width / 2
                    inputMethodHints: Qt.ImhFormattedNumbersOnly
                    onDisplayTextChanged: {
                        if (displayText !== block.attr("ipAddress").val) {
                            block.attr("ipAddress").val = displayText
                        }
                    }
                }
            }
            BlockRow {
                Text {
                    text: "Port:"
                    width: parent.width - 90*dp
                }
                AttributeNumericInput {
                    width: 90*dp
                    implicitWidth: 0
                    attr: block.attr("port")
                }
            }
            BlockRow {
                Text {
                    text: "Password:"
                    width: parent.width / 2
                }
                TextInput {
                    text: block.attr("password").val
                    width: parent.width / 2
                    inputMethodHints: Qt.ImhPreferLatin
                    clip: true
                    onDisplayTextChanged: {
                        if (displayText !== block.attr("password").val) {
                            block.attr("password").val = displayText
                        }
                    }
                }
            }
            BlockRow {
                ButtonBottomLine {
                    text: "Reset Connection"
                    onPress: block.resetConnection()
                }
            }
        }
    }  // end Settings Component
}

