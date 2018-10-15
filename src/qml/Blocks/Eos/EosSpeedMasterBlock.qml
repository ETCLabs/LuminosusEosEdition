import QtQuick 2.5
import CustomElements 1.0
import CustomStyle 1.0
import "../../CustomBasics"
import "../../CustomControls"

BlockBase {
    id: root
    width: 280*dp
    height: Math.max(150*dp, 120*dp + block.effectNumbers.length * 30*dp) + 2
    onHeightChanged: block.positionChanged()
    settingsComponent: settings

    StretchColumn {
        anchors.fill: parent

        StretchRow {
            height: 60*dp
            StretchText {
                font.pixelSize: 20*dp
                hAlign: Text.AlignHCenter
                text: "<b>" + block.attr("bpm").val.toFixed(1) + "</b> BPM"
                textFormat: Text.RichText
            }
            Item {
                width: 60*dp
                Rectangle {
                    anchors.centerIn: parent
                    width: 20*dp
                    height: 20*dp
                    radius: width / 2
                    color: Style.primaryActionColor

                    SequentialAnimation on opacity {
                        id: opacityAnimation
                        PropertyAction {
                            value: 1
                        }
                        PauseAnimation {
                            duration: 100
                        }
                        NumberAnimation {
                            to: 0
                            duration: 100
                        }
                    }

                    Timer {
                        interval: 50  // set to real value on each interval
                        running: true
                        repeat: true
                        onTriggered: {
                            interval = 60000 / Math.max(block.attr("bpm").val, 1)
                            opacityAnimation.restart()
                        }
                    }
                }
            }
        }

        Rectangle {
            height: 1
            color: "#888"
        }

        Repeater {
            id: effectNumbersRepeater
            model: block.effectNumbers

            BlockRow {
                id: effectRow
                property real multiplier: block.getMultiplier(index)
                Connections {
                    target: block
                    onMultipliersChanged: effectRow.multiplier = block.getMultiplier(index)
                }

                IconButton {
                    width: 30*dp
                    iconName: "trash_icon"
                    clickDurationEnabled: true
                    onShortClick: guiManager.showToast("Long click to delete effect")
                    onLongClick: {
                        block.removeEffect(index)
                    }
                }

                StretchText {
                    hAlign: Text.AlignHCenter
                    text: modelData
                }

                ButtonSideLine {
                    width: 30*dp
                    implicitWidth: 0
                    text: "/2"
                    onPress: block.setMultiplier(index, 0.5)
                    marked: effectRow.multiplier === 0.5
                    mappingID: block.getUid() + modelData + "/2"
                    allUpperCase: false
                }
                ButtonSideLine {
                    width: 30*dp
                    implicitWidth: 0
                    text: "x1"
                    onPress: block.setMultiplier(index, 1.0)
                    marked: effectRow.multiplier === 1.0
                    mappingID: block.getUid() + modelData + "x1"
                    allUpperCase: false
                }
                ButtonSideLine {
                    width: 30*dp
                    implicitWidth: 0
                    text: "x2"
                    onPress: block.setMultiplier(index, 2.0)
                    marked: effectRow.multiplier === 2.0
                    mappingID: block.getUid() + modelData + "x2"
                    allUpperCase: false
                }
                ButtonSideLine {
                    width: 30*dp
                    implicitWidth: 0
                    text: "x4"
                    onPress: block.setMultiplier(index, 4.0)
                    marked: effectRow.multiplier === 4.0
                    mappingID: block.getUid() + modelData + "x4"
                    allUpperCase: false
                }
            }
        }

        StretchText {
            visible: effectNumbersRepeater.count === 0
            hAlign: Text.AlignHCenter
            text: "No effects controlled"
        }

        Rectangle {
            height: 1
            color: "#888"
        }

        BlockRow {
            leftMargin: 10*dp
            StretchText {
                text: "Add Effect:"
            }

            NumericInput {
                id: effectNumberInput
                centered: true
                minimumValue: 1
                maximumValue: 9999
                decimals: 2
                value: 1.0
            }

            IconButton {
                width: 30*dp
                iconName: "plus_icon"
                onPress: {
                    block.addEffect(effectNumberInput.value)
                    effectNumberInput.value = 1.0
                }
            }
        }

        DragArea {
            text: "Speed Master"
            InputNodeRect {
                node: block.node("inputNode")
                suggestions: ["Beat Detection", "BPM", "Crossfade"]
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
                StretchText {
                    text: "Send regularly:"
                }
                AttributeCheckbox {
                    width: 30*dp
                    attr: block.attr("sendRegularly")
                }
            }
        }
    }  // end Settings Component
}

