import QtQuick 2.5
import CustomElements 1.0
import "../CustomBasics"
import "../CustomControls"

BlockBase {
    id: root
    width: 190*dp
    height: 170*dp

	StretchColumn {
        anchors.fill: parent

        StretchRow {
            implicitHeight: -1

            CustomTouchArea {
                id: touchArea
                implicitWidth: -1
                Image {
                    anchors.centerIn: parent
                    width: 132*dp
                    height: 132*dp
                    source: "qrc:/images/color_wheel_thick@2x.png"
                }
                Image {
                    id: cursor
                    width: 26*dp
                    height: 26*dp
                    x: 18*dp
                    y: 18*dp
                    source: "qrc:/images/color_wheel_cursor@2x.png"
                }
                Rectangle {
                    width: 50*dp
                    height: 50*dp
                    radius: width / 2
                    anchors.centerIn: parent
                    gradient: Gradient {
                        GradientStop { position: 0.0; color: Qt.hsla(block.hue, block.sat, 0.6 + (1 - block.sat) * 0.4, 1) }
                        GradientStop { position: 1.0; color: Qt.hsla(block.hue, block.sat, 0.4 + (1 - block.sat) * 0.3, 1) }
                    }

                }

                onTouchDown: updateCursor(touch)
                onTouchMove: updateCursor(touch)
                onTouchUp: updateCursor(touch)

                function updateCursor(touch) {
                    var angle = getAngle(touch)
                    block.hue = 1 - ((angle / (2*Math.PI)) % 1)
                }

                function getAngle(touch) {
                    // angle is in radian (0-2*pi)
                    var centerX = x + width / 2
                    var centerY = y + height / 2
                    var angle = Math.atan2((touch.itemX - centerX), (touch.itemY - centerY))
                    if (angle < 0) {
                        angle = (2 * Math.PI) + angle
                    }
                    angle = 2*Math.PI - angle
                    angle += Math.PI / 2
                    return angle
                }

                Connections {
                    target: block
                    onHueChanged: touchArea.setCursorPosition(block.hue)
                }
                Component.onCompleted: touchArea.setCursorPosition(block.hue)
                onHeightChanged: touchArea.setCursorPosition(block.hue)

                function setCursorPosition(hue) {
                    // angle is in radian (0-2*pi)
                    // angle = 0 is in the middle right
                    // increase angle is clockwise rotation
                    var angle = (1 - hue) * Math.PI * 2
                    var centerX = x + width / 2
                    var centerY = y + height / 2
                    var radius = 57*dp
                    cursor.x = centerX + radius * Math.cos(angle) - cursor.width / 2
                    cursor.y = centerY + radius * Math.sin(angle) - cursor.height / 2
                }

            }  // end CustomTouchArea

            Item {
                width: 20*dp
                Rectangle {
                    gradient: Gradient {
                            GradientStop { position: 0.0; color: Qt.hsla(block.hue, 1, 0.5, 1) }
                            GradientStop { position: 1.0; color: "white" }
                        }
                    anchors.fill: parent
                    anchors.topMargin: 20*dp
                    anchors.bottomMargin: 20*dp
                    anchors.leftMargin: 8*dp
                    anchors.rightMargin: 8*dp
                }
            }

            Slider {
                width: 30*dp
                value: block.sat
                onValueChanged: {
                    if (value !== block.sat) {
                        block.sat = value;
                    }
                }
            }
        }  // end StretchRow

        DragArea {
            InputNode {
                objectName: "inputNode"
            }

			OutputNode {
				objectName: "outputNode"
			}
		}
    }
}

