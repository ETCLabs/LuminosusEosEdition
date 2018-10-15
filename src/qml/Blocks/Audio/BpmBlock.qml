import QtQuick 2.5
import CustomElements 1.0
import "../../CustomBasics"
import "../../CustomControls"

BlockBase {
    id: root
    width: 120*dp
    height: 150*dp
    settingsComponent: settings

    StretchColumn {
        anchors.fill: parent

        Button {
            height: 90*dp
            text: "Tap"
            onPress: block.triggerBeat()
            mappingID: block.getUid() + "tap"

            property real midiFeedbackValue: 0.0
            onMidiFeedbackValueChanged: controller.midiMapping().sendFeedback(mappingID, midiFeedbackValue)

            SequentialAnimation on midiFeedbackValue {
                id: midiFeedbackAnimation
                PropertyAction {
                    value: true
                }
                PauseAnimation {
                    duration: 150
                }
                PropertyAction {
                    value: false
                }
            }

            Timer {
                interval: 60000 / Math.max(block.bpm, 1)
                running: true
                repeat: true
                onTriggered: midiFeedbackAnimation.restart()
            }

            Text {
                anchors.centerIn: parent
                anchors.verticalCenterOffset: 20*dp
                font.pixelSize: 12*dp
                text: block.bpm.toFixed(1) + " BPM"
            }
        }
        BlockRow {
            ButtonSideLine {
                text: "/2"
                onPress: block.factor = 0.5
                marked: block.factor === 0.5
                mappingID: block.getUid() + "/2"
                allUpperCase: false
            }
            ButtonSideLine {
                text: "x1"
                onPress: block.factor = 1
                marked: block.factor === 1
                mappingID: block.getUid() + "x1"
                allUpperCase: false
            }
            ButtonSideLine {
                text: "x2"
                onPress: block.factor = 2
                marked: block.factor === 2
                mappingID: block.getUid() + "x2"
                allUpperCase: false
            }
            ButtonSideLine {
                text: "x4"
                onPress: block.factor = 4
                marked: block.factor === 4
                mappingID: block.getUid() + "x4"
                allUpperCase: false
            }
        }

        DragArea {
            text: "BPM"
            InputNode {
                node: block.node("inputNode")
            }
            OutputNodeRect {
                node: block.node("outputNode")
                suggestions: ["Eos Effect", "Eos Speed Master", "Linear Value", "Sinus Value", "Random Value", "Tick Generator"]
            }
        }
    }

    Component {
        id: settings
        StretchColumn {
            leftMargin: 15*dp
            rightMargin: 15*dp
            defaultSize: 30*dp

            BlockRow {
                StretchText {
                    text: "Manual BPM:"
                }
                NumericInput {
                    width: 120*dp
                    implicitWidth: 0
                    minimumValue: 1
                    maximumValue: 999
                    decimals: 1
                    value: block.bpm
                    onValueChanged: {
                        if (value !== block.bpm) {
                            block.bpm = value
                        }
                    }
                }
            }  // end BPM range

        }  // end settings column
    }
}

