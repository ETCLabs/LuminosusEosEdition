import QtQuick 2.5
import QtQuick.Window 2.2
import CustomElements 1.0
import "../CustomBasics"
import "../CustomControls"

BlockBase {
    id: root
    width: 600*dp
    height: 220*dp
    settingsComponent: settings

    StretchColumn {
        anchors.fill: parent

        Item {  // Spectrum Area
            implicitHeight: -1

            SpectralHistoryItem {
                id: spectralHistory
                anchors.fill: parent
                color: Qt.rgba(0.25, 0.45, 0.8, 1)

                analyzer: block.analyzer
            }
        }
        DragArea {
            text: "Tempo Detection"

            BlockRow {
                height: 30*dp
                width: 4*30*dp
                ButtonSideLine {
                    text: "/2"
                    onPress: block.factor = 0.5
                    marked: block.factor === 0.5
                }
                ButtonSideLine {
                    text: "1x"
                    onPress: block.factor = 1
                    marked: block.factor === 1
                }
                ButtonSideLine {
                    text: "2x"
                    onPress: block.factor = 2
                    marked: block.factor === 2
                }
                ButtonSideLine {
                    text: "4x"
                    onPress: block.factor = 4
                    marked: block.factor === 4
                }
            }

            Text {
                anchors.right: parent.right
                anchors.rightMargin: 15*dp
                height: parent.height
                font.bold: true
                text: block.bpm.toFixed(1) + " BPM"
                color: block.bpmIsValid ? "white" : "#888"
            }

            OutputNode {
                objectName: "outputNode"
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
                Text {
                    text: "Inputs:"
                    width: parent.width
                }
            }
            ListView {
                id: list
                width: parent.width
                height: count * 30*dp
                model: controller.audioEngine().getDeviceNameList()
                // prevent async buffering that can cause segfaults:
                cacheBuffer: 0
                delegate: Row {
                    width: parent.width
                    height: 30*dp
                    onVisibleChanged: {
                        if (visible) {
                            timer.start()
                        } else {
                            timer.stop()
                        }
                    }
                    ButtonSideLine {
                        id: button
                        width: parent.width
                        height: 30*dp
                        text: modelData
                        onPress: block.setInputByName(modelData)
                        marked: modelData === block.getInputName()
                        Connections {
                            target: block
                            onInputChanged: button.marked = modelData === block.getInputName()
                        }

                        Rectangle {
                            id: bar
                            x: 0
                            y: 0
                            height: parent.height
                            width: 0
                            color: Qt.rgba(1, 1, 1, 0.15)
                        }
                        Timer {
                            id: timer
                            interval: 50; running: true; repeat: true
                            onTriggered: {
                                var factor = controller.audioEngine().getMaxLevelOfDevice(modelData)
                                bar.width = parent.width * factor
                            }
                        }
                    }
                }
            } // end list view

            BlockRow {
                Text {
                    text: "BPM Range:"
                    width: 100*dp
                }
                ComboBox2 {
                    implicitWidth: -1
                    values: [0, 50, 75, 100, 150]
                    texts: ["Auto", "50 - 99", "75 - 149", "100 - 199", "150 - 299"]
                    property bool initialized: false
                    Component.onCompleted: {
                        setValue(block.minBpm)
                        initialized = true
                    }
                    onValueChanged: {
                        if (!initialized) return
                        if (value !== block.minBpm) {
                            block.minBpm = value
                        }
                    }
                }
            }  // end BPM range

        }  // end settings column
    }
}

