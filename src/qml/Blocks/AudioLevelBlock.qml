import QtQuick 2.5
import QtQuick.Window 2.2
import CustomElements 1.0
import "../CustomBasics"
import "../CustomControls"

BlockBase {
    id: root
    width: 256*dp
    height: width*0.5
    settingsComponent: settings

	StretchColumn {
        anchors.fill: parent

        Item {  // Spectrum Area
            implicitHeight: -1

            AudioSpectrumItem {
                id: spectrum
                anchors.fill: parent
                color: Qt.rgba(0.25, 0.45, 0.8, 1)

                analyzer: block.analyzer
                agcEnabled: block.agcEnabled
            }

            Image {
                source: "qrc:/images/spectrum_gradient.png"
                anchors.fill: parent
            }

            Item {  // Legend at the bottom
                height: 10*dp
                anchors.bottom: parent.bottom
                Text {
                    text: "20"
                    font.pixelSize: 10*dp
                    x: root.width * 0.05
                    y: -1*dp
                }
                Text {
                    text: "200"
                    font.pixelSize: 10*dp
                    x: root.width * 0.38
                    y: -1*dp
                }
                Text {
                    text: "1k"
                    font.pixelSize: 10*dp
                    x: root.width * 0.6
                    y: -1*dp
                }
                Text {
                    text: "10k"
                    font.pixelSize: 10*dp
                    x: root.width * 0.9
                    y: -1*dp
                }
            }

            Rectangle {  // Red Line, Band Marker
                width: 1*dp
                height: parent.height
                x: block.currentBand * parent.width
                color: "red"
            }
            CustomTouchArea {  // Band Marker Interaction
                anchors.fill: parent
                onTouchDown: {
                    var band = touch.itemX / width
                    block.setCurrentBand(band)
                }
                onTouchMove: {
                    var band = touch.itemX / width
                    block.setCurrentBand(band)
                }
            }
        }

        DragArea {
            height: 30*dp
            implicitHeight: 0  // do not stretch
			text: "Audio"

            WidthResizeAreaLeft {
                target: root
                minSize: 250*dp
                maxSize: 700*dp
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
            }
        }
    }
}

