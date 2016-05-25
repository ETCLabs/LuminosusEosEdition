import QtQuick 2.5
import CustomElements 1.0
import "../CustomBasics"
import "../CustomControls"

BlockBase {
    id: root
    width: 140*dp
    height: 170*dp
    pressed: dragarea.pressed

    property real hue: 0.0

	StretchColumn {
        anchors.fill: parent

		TouchArea {
			implicitHeight: -1
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
                    GradientStop { position: 0.0; color: Qt.hsla(hue, 1, 0.7, 1) }
                    GradientStop { position: 1.0; color: Qt.hsla(hue, 1, 0.4, 1) }
                }

            }
            Rectangle {
                width: 57*dp
                height: 57*dp
                radius: width / 2
                anchors.centerIn: parent
                color: Qt.hsla(hue, 1, 0.5, 1)
                visible: false
            }

            onTouchDown: updateCursor(touch)
            onTouchMove: updateCursor(touch)
            onTouchUp: updateCursor(touch)

            function updateCursor(touch) {
                var angle = getAngle(touch)
                hue = 1 - (((angle / (2*Math.PI)) - 0.25) % 1)
                setCursorPosition(angle)
            }

            function getAngle(touch) {
                // angle is in radian (0-2*pi)
                var centerX = x + width / 2
                var centerY = y + height / 2
                var angle = Math.atan2((touch.x - centerX), (touch.y - centerY))
                if (angle < 0) {
                    angle = (2 * Math.PI) + angle
                }
                angle = 2*Math.PI - angle
                angle += Math.PI / 2
                return angle
            }

            function setCursorPosition(angle) {
                // angle is in radian (0-2*pi)
                // angle = 0 is in the middle right
                // increase angle is clockwise rotation
                var centerX = x + width / 2
                var centerY = y + height / 2
                var radius = 57*dp
                cursor.x = centerX + radius * Math.cos(angle) - cursor.width / 2
                cursor.y = centerY + radius * Math.sin(angle) - cursor.height / 2
            }

        }

		DragArea {
			id: dragarea
			guiBlock: root

			OutputNode {
				objectName: "outputNode"
			}
		}
    }
}

