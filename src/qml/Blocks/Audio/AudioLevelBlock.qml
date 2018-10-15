import QtQuick 2.5
import QtQuick.Window 2.2
import CustomElements 1.0
import "../../CustomBasics"
import "../../CustomControls"

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
                lineWidth: 2*dp

                analyzer: block.analyzer
                agcEnabled: block.agcEnabled
                manualGain: block.manualGain

                visible: !block.attr("showSpectrogram").val && width < 500*dp
            }

            AudioBarSpectrumItem {
                anchors.fill: parent
                color: Qt.rgba(0.3, 0.5, 1.0, 1)
                lineWidth: 2*dp

                analyzer: block.analyzer
                agcEnabled: block.agcEnabled
                manualGain: block.manualGain

                visible: !block.attr("showSpectrogram").val && width >= 500*dp
            }

            ImagePainter {
                anchors.fill: parent
                image: block.spectrogram
                smooth: false

                visible: block.attr("showSpectrogram").val
            }

            Rectangle {  // Yellow Area, Band Marker
                width: Math.min(parent.width * block.bandwidth, parent.width - x)
                height: parent.height * block.upperBound
                x: Math.min(parent.width - 2*dp, block.currentBand * parent.width)
                y: parent.height * (1 - block.upperBound)
                color: touchArea.pressed ? Qt.rgba(0.9, 0.9, 0, 0.3) : Qt.rgba(0.9, 0.9, 0.9, 0.3)
                Behavior on color {
                    ColorAnimation {
                        duration: 300
                        easing.type: Easing.OutCubic
                    }
                }

                border.width: 1*dp
                border.color: Qt.rgba(0.9, 0.9, 0, 0.8)
            }

            Rectangle {  // frequency mode marker
                width: Math.min(parent.width * block.bandwidth * block.peakPosition, parent.width - x)
                height: 2*dp
                x: Math.min(parent.width - 2*dp, block.currentBand * parent.width)
                y: parent.height * (1 - block.upperBound) - 1*dp
                color: "red"
                visible: block.frequencyMode && !block.attr("showSpectrogram").val
            }

            Rectangle {  // frequency mode marker in spectrogram mode
                width: 2*dp
                height: parent.height
                x: block.currentBand * parent.width + parent.width * block.bandwidth * block.peakPosition
                y: 0
                color: "red"
                visible: block.frequencyMode && block.attr("showSpectrogram").val
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

            CustomTouchArea {  // Band Marker Interaction
                id: touchArea
                anchors.fill: parent
                onTouchDown: {
                    var band = (touch.itemX - width * block.bandwidth / 2) / width
                    block.setCurrentBand(band)
                    var uppderBound = 1 - (touch.itemY / height)
                    block.setUpperBound(uppderBound)
                }
                onTouchMove: {
                    var band = (touch.itemX - width * block.bandwidth / 2) / width
                    block.setCurrentBand(band)
                    var uppderBound = 1 - (touch.itemY / height)
                    block.setUpperBound(uppderBound)
                }
            }
        }

        DragArea {
            height: 30*dp
            implicitHeight: 0  // do not stretch
			text: "Audio"

            WidthResizeAreaLeft {
                target: root
                minSize: 150*dp
                maxSize: 1000*dp
            }

            OutputNode {
                node: block.node("outputNode")
                suggestions: ["Threshold", "Smooth"]
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
                    id: agcCheckbox
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
                implicitHeight: 0  // do not stretch
                height: 120*dp
                visible: !agcCheckbox.active
                Text {
                    text: "Gain: " + block.manualGain.toFixed(1) + "x"
                    width: parent.width - 30*dp
                }
                Slider {
                    width: 30*dp
                    value: block.manualGainNorm
                    onValueChanged: {
                        if (value !== block.manualGainNorm) {
                            block.manualGainNorm = value
                        }
                    }
                }
            }

            BlockRow {
                StretchText {
                    text: "Width [Oct.]:"
                }
                NumericInput {
                    width: 60*dp
                    implicitWidth: 0  // do not stretch
                    minimumValue: 0.1
                    maximumValue: 10
                    decimals: 1
                    value: block.bandwidth * 10
                    onValueChanged: {
                        if (Math.abs((value / 10) - block.bandwidth) > 0.01) {
                            block.bandwidth = (value / 10)
                        }
                    }
                }
            }

            BlockRow {
                Text {
                    text: "Frequency Mode:"
                    width: parent.width - 30*dp
                }
                CheckBox {
                    width: 30*dp
                    active: block.frequencyMode
                    onActiveChanged: {
                        if (active !== block.frequencyMode) {
                            block.frequencyMode = active
                        }
                    }
                }
            }

            BlockRow {
                Text {
                    text: "Spectrogram:"
                    width: parent.width - 30*dp
                }
                AttributeCheckbox {
                    width: 30*dp
                    attr: block.attr("showSpectrogram")
                }
            }

            BlockRow {
                Text {
                    text: "Input:"
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
            }
        }
    }
}

