import QtQuick 2.5
import QtQuick.Window 2.2
import CustomElements 1.0
import "../../CustomBasics"
import "../../CustomControls"

BlockBase {
    id: root
    width: 150*dp
    height: 90*dp
    settingsComponent: settings

    StretchColumn {
        anchors.fill: parent

        BlockRow {
            id: fluxRow
            Item {
                width: 0
                Rectangle {
                    height: fluxRow.height
                    width: fluxRow.width * block.spectralFlux * block.fluxGain
                    color: Qt.rgba(1, 1, 1, 0.1)
                }
            }
            StretchText {
                text: "Spectral Flux:"
                hAlign: Text.AlignHCenter
            }
            OutputNode {
                node: block.node("fluxNode")
                suggestions: ["Threshold", "Smooth"]
            }
        }
        BlockRow {
            id: volumeRow
            Item {
                width: 0
                Rectangle {
                    height: volumeRow.height
                    width: volumeRow.width * block.volume * block.volumeGain
                    color: Qt.rgba(1, 1, 1, 0.1)
                }
            }
            StretchText {
                text: "Volume:"
                hAlign: Text.AlignHCenter
            }
            OutputNode {
                node: block.node("outputNode")
                suggestions: ["Threshold", "Smooth"]
            }
        }
        DragArea {
            text: "Audio"
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
                    text: "Automatic Gain:"
                    width: parent.width - 30*dp
                }
                CheckBox {
                    width: 30*dp
                    active: block.agcEnabled
                    onActiveChanged: {
                        if (active !== block.agcEnabled) {
                            block.agcEnabled = active
                        }
                    }
                }
            }

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
                        wrap: true
                        allUpperCase: false
                        text_area.horizontalAlignment: Text.AlignLeft
                        text_area.anchors.leftMargin: 5*dp
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

        }  // end settings column
    }
}

