import QtQuick 2.5
import CustomElements 1.0
import "../../CustomBasics"
import "../../CustomControls"

BlockBase {
    id: root
    width: 170*dp
    height: 170*dp

    property QtObject hue: block.attr("hue")
    property QtObject sat: block.attr("sat")

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
                        GradientStop { position: 0.0; color: Qt.hsla(hue.val, sat.val, 0.6 + (1 - sat.val) * 0.4, 1) }
                        GradientStop { position: 1.0; color: Qt.hsla(hue.val, sat.val, 0.4 + (1 - sat.val) * 0.3, 1) }
                    }

                }

                onTouchDown: {
                    controller.midiMapping().guiControlHasBeenTouched(mappingID)
                    updateCursor(touch)
                }
                onTouchMove: updateCursor(touch)
                onTouchUp: updateCursor(touch)

                function updateCursor(touch) {
                    var angle = getAngle(touch)
                    hue.val = 1 - ((angle / (2*Math.PI)) % 1)
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
                    target: hue
                    onValueChanged: touchArea.setCursorPosition(hue.val)
                }
                Component.onCompleted: {
                    controller.midiMapping().registerGuiControl(this, mappingID)
                    touchArea.setCursorPosition(hue.val)
                }
                onHeightChanged: touchArea.setCursorPosition(hue.val)

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

                // ---------------------- External Input Handling ----------------------

                property string mappingID: block.getUid() + hue.name()
                property real externalInput: 0
                Component.onDestruction: if (controller) controller.midiMapping().unregisterGuiControl(mappingID)
                onExternalInputChanged: {
                    hue.val = externalInput
                }
                Connections {
                    target: hue
                    onValChanged: controller.midiMapping().sendFeedback(parent.mappingID, hue.val)
                }

            }  // end CustomTouchArea

            SaturationSlider {
                width: 30*dp
                property QtObject attr: block.attr("sat")
                value: attr.val
                onValueChanged: {
                    if (value !== attr.val) {
                        attr.val = value
                    }
                }
                hue: root.hue.val
                mappingID: attr.block().getUid() + attr.name()
            }
        }  // end StretchRow

        DragArea {
            InputNode {
                node: block.node("inputNode")
                suggestions: ["Slider"]
            }

            OutputNode {
                node: block.node("outputNode")
			}
		}
    }
}

