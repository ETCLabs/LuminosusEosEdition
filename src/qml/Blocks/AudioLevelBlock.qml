import QtQuick 2.5
import QtQuick.Window 2.2
import CustomElements 1.0
import "../CustomBasics"
import "../CustomControls"

BlockBase {
    id: root
    width: 256*dp
    height: 140*dp
    pressed: dragarea.pressed
    settingsComponent: settings

	StretchColumn {
        anchors.fill: parent

        Item {
			height: 100*dp
            Canvas {
                id: canvas
                property real scaling: Screen.devicePixelRatio
                height: parent.height*scaling
                width: parent.width*scaling
                scale: 1 / scaling
                x: -parent.width * (1 - (1 / scaling))
                y: -parent.height * (1 - (1 / scaling))
                onPaint: {
                    // --- Prepare ---
                    var points = block.getSpectrumPoints()
                    var pointCount = points.length;
                    var pointWidth = width / pointCount
                    var ctx = canvas.getContext('2d');
                    ctx.clearRect (0, 0, width, height);

                    // --- Spectrum Geometry ---
                    ctx.beginPath();
                    ctx.moveTo(0, height);
                    for (var i=0; i<pointCount; i++) {
                        ctx.lineTo(pointWidth*i, height * (1 - points[i]))
                        //ctx.lineTo(pointWidth*i, height * (1 - Math.random()))
                    }
                    ctx.lineTo(width, height);

                    // --- Fill and Stroke Spectrum ---
                    var gradient = ctx.createLinearGradient(0, 0, 0, height);
                    gradient.addColorStop(0., '#517EA8');  // top
                    gradient.addColorStop(1., '#365470');  // bottom
                    ctx.fillStyle = gradient;
                    ctx.fill();
                    ctx.lineWidth = 1*dp*scaling
                    ctx.lineJoin = "bevel"
                    ctx.strokeStyle = "#ddd"
                    ctx.stroke();

                    // --- Band Marker ---
                    ctx.beginPath();
                    ctx.moveTo(width*block.getCurrentBand(), 0);
                    ctx.lineTo(width*block.getCurrentBand(), height);
                    gradient = ctx.createLinearGradient(0, 0, 0, height);
                    gradient.addColorStop(0., '#e00');  // top
                    gradient.addColorStop(0.5, '#f44');  // mid
                    gradient.addColorStop(1., '#b00');  // bottom
                    ctx.strokeStyle = gradient;
                    ctx.stroke();
                }
                Timer {
                    interval: 20; running: true; repeat: true
                    onTriggered: canvas.requestPaint()
                }
            }
            TouchArea {
                anchors.fill: parent
                onTouchDown: {
                    var band = touch.x / width
                    block.setCurrentBand(band)
                }
                onTouchMove: {
                    var band = touch.x / width
                    block.setCurrentBand(band)
                }
            }
        }

        Rectangle {
			height: 10*dp
            color: "#365470"
            //color: "transparent"
            Text {
                text: "20"
                font.pixelSize: 10*dp
                y: -1*dp
            }
            Text {
                text: "200"
                font.pixelSize: 10*dp
                x: 70*dp
                y: -1*dp
            }
            Text {
                text: "1k"
                font.pixelSize: 10*dp
                x: 137*dp
                y: -1*dp
            }
            Text {
                text: "10k"
                font.pixelSize: 10*dp
                x: 220*dp
                y: -1*dp
            }
        }

		DragArea {
			id: dragarea
			guiBlock: root
			text: "Audio"
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
                model: controller.audioEngine().getInputList()
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
                                var factor = controller.audioEngine().getMaxLevelByName(modelData)
                                bar.width = parent.width * factor
                            }
                        }
                    }
                }
            }
        }
    }
}

