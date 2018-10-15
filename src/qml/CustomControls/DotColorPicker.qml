import QtQuick 2.5
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0
import CustomStyle 1.0
import CustomElements 1.0

import "../CustomBasics"

Item {
    id: root

    // ------------------------- Public Properties ------------------
    property color col: Qt.hsv(0, 0, 0)


    // ------------------------- Private Properties ------------------
    QtObject {
        id: priv
        property bool justClosed: false
    }

    // ------------------------ Label + Background -------------------

    Rectangle {
        anchors.fill: parent
        anchors.margins: 1*dp
        radius: Math.max(width, height) / 2

        color: col
    }
    Rectangle {
        anchors.centerIn: parent
        width: 6*dp
        height: 6*dp
        radius: 3*dp
        color: Style.primaryActionColor
    }


    // ------------------------ NumBlock Handling ----------------------

    property Item numBlockItem

    CustomTouchArea {
        anchors.fill: parent

        onClick: {
            // check if NumBlock is open:
            if (numBlockItem) {
                controller.playClickSound()
                // it is open -> destroy it:
                numBlockItem.destroy()
                return
            }
            // don't change anything if not enabled:
            if (!root.enabled) return

            controller.playClickSound()

            // ------------------ Set Preferred Local Coordinates -------------------
            // preffered horizontal position is centered above the middle of the NumericInput:
            var preferredLocalX = (root.width / 2) - (numBlockWidth / 2)
            // if NumericInput is in the lower half of the window, show the input above
            var prefferedLocalY = -numBlockHeight
            // if NumericInput is in the upper half of the window, show the input below
            if (root.mapToItem(null, 0, 0).y < (Window.height / 2)) {
                prefferedLocalY = root.height
            }

            // ---------- Translate Local Coordinates to Window Coordiantes -----------
            var windowCoords = root.mapToItem(null, preferredLocalX, prefferedLocalY)
            var winX = windowCoords.x
            var winY = windowCoords.y

            // --------------- Check bounds to be visible ----------------
            var window = Window.contentItem
            winX = Math.max(0, Math.min(window.width - numBlockWidth, winX))
            winY = Math.max(0, Math.min(window.height - numBlockHeight, winY))


            // -------------------- Show Component -------------------
            numBlockItem = numBlockComponent.createObject(window, {x: winX, y: winY})
            numBlockItem.forceActiveFocus()
        }
    }

    // ------------------------ Color Picker (NumBlock) Component ------------------------

    property real numBlockWidth: (200 + 2)*dp  // 1px border * 2
    property real numBlockHeight: (140 + 2)*dp  // 1px border * 2

    Component {
        id: numBlockComponent

        Item {
            width: numBlockWidth
            height: numBlockHeight

            RectangularGlow {
                id: effect
                anchors.fill: rect
                glowRadius: 10
                spread: 0.2
                color: "black"
                cornerRadius: rect.radius + glowRadius
            }

            Rectangle {
                id: rect
                color: "#444"
//                border.width: 1
//                border.color: "#aaa"
                radius: 8*dp
                anchors.fill: parent

                onActiveFocusChanged: {
                    if (!activeFocus) {
                        numBlockItem.destroy()
                    }
                }

                StretchRow {
                    id: numBlock
                    anchors.fill: parent
                    anchors.margins: 1*dp

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
                                GradientStop { position: 0.3; color: Qt.hsva(root.col.hsvHue, root.col.hsvSaturation, root.col.hsvValue, 1) }
                                GradientStop { position: 1.0; color: Qt.hsva(root.col.hsvHue, root.col.hsvSaturation, root.col.hsvValue * 0.7, 1) }
                            }

                        }

                        onTouchDown: {
                            controller.midiMapping().guiControlHasBeenTouched(extInputH.mappingID)
                            updateHue(touch)
                        }
                        onTouchMove: updateHue(touch)
                        onTouchUp: updateHue(touch)

                        function updateHue(touch) {
                            var angle = getAngle(touch)
                            root.col.hsvHue = 1 - ((angle / (2*Math.PI)) % 1)
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
                            target: root
                            onColChanged: touchArea.setCursorPosition(root.col.hsvHue)
                        }
                        Component.onCompleted: {
                            touchArea.setCursorPosition(root.col.hsvHue)
                        }
                        onHeightChanged: touchArea.setCursorPosition(root.col.hsvHue)

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

                    SaturationSlider {
                        width: 30*dp
                        value: root.col.hsvSaturation
                        onValueChanged: {
                            if (value !== root.col.hsvSaturation) {
                                root.col.hsvSaturation = value
                            }
                        }
                        hue: root.col.hsvHue
                        midiMappingEnabled: false
                        onTouchDown: controller.midiMapping().guiControlHasBeenTouched(extInputS.mappingID)
                    }

                    BrightnessSlider {
                        width: 30*dp
                        value: root.col.hsvValue
                        onValueChanged: {
                            if (value !== root.col.hsvValue) {
                                root.col.hsvValue = value
                            }
                        }
                        midiMappingEnabled: false
                        onTouchDown: controller.midiMapping().guiControlHasBeenTouched(extInputV.mappingID)
                    }
                }  // end StretchRow
            }  // background Rectangle end
        }// end Item
    }  // Component numBlockComponent


    // ---------------------- External Input Handling ----------------------

    property string mappingID
    Item {
        id: extInputH
        property string mappingID: root.mappingID + "H"
        property real externalInput: -1
        Component.onCompleted: controller.midiMapping().registerGuiControl(this, mappingID)
        Component.onDestruction: if (controller) controller.midiMapping().unregisterGuiControl(mappingID)
        onExternalInputChanged: {
            root.col.hsvHue = externalInput
        }
        Connections {
            target: root
            onColChanged: controller.midiMapping().sendFeedback(extInputH.mappingID, root.col.hsvHue)
        }
    }
    Item {
        id: extInputS
        property string mappingID: root.mappingID + "S"
        property real externalInput: -1
        Component.onCompleted: controller.midiMapping().registerGuiControl(this, mappingID)
        Component.onDestruction: if (controller) controller.midiMapping().unregisterGuiControl(mappingID)
        onExternalInputChanged: {
            root.col.hsvSaturation = externalInput
        }
        Connections {
            target: root
            onColChanged: controller.midiMapping().sendFeedback(extInputS.mappingID, root.col.hsvSaturation)
        }
    }
    Item {
        id: extInputV
        property string mappingID: root.mappingID + "V"
        property real externalInput: -1
        Component.onCompleted: controller.midiMapping().registerGuiControl(this, mappingID)
        Component.onDestruction: if (controller) controller.midiMapping().unregisterGuiControl(mappingID)
        onExternalInputChanged: {
            root.col.hsvValue = externalInput
        }
        Connections {
            target: root
            onColChanged: controller.midiMapping().sendFeedback(extInputV.mappingID, root.col.hsvValue)
        }
    }
    Component.onCompleted: {
        if (root.col.hsvHue < 0) {
            // if hue cannot be determined, it is set to -1 by Qt
            // in this case, set it to 0:
            root.col.hsvHue = 0
        }
        controller.midiMapping().sendFeedback(extInputH.mappingID, root.col.hsvHue)
        controller.midiMapping().sendFeedback(extInputS.mappingID, root.col.hsvSaturation)
        controller.midiMapping().sendFeedback(extInputV.mappingID, root.col.hsvValue)
    }
}
