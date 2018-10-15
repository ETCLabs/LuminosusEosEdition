import QtQuick 2.5
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0
import CustomStyle 1.0
import CustomElements 1.0

import "../CustomBasics"

Item {
    id: root

    // ------------------------- Public Properties ------------------
    property QtObject attr


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
        antialiasing: dp <= 1

        color: attr.qcolor
    }
    Rectangle {
        anchors.centerIn: parent
        width: 6*dp
        height: 6*dp
        radius: 3*dp
        antialiasing: dp <= 1
        color: Style.primaryActionColor
    }


    // ------------------------ NumBlock Handling ----------------------

    property Item numBlockItem

    CustomTouchArea {
        anchors.fill: parent

        onClick: {
            // check if NumBlock is open:
            if (numBlockItem) {
                controller.playClickUpSound()
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

            onActiveFocusChanged: {
                if (!activeFocus) {
                    numBlockItem.destroy()
                }
            }

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
                radius: 8*dp
                antialiasing: dp <= 1
                anchors.fill: parent

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
                                GradientStop { position: 0.3; color: Qt.hsva(attr.hue, attr.sat, attr.val, 1) }
                                GradientStop { position: 1.0; color: Qt.hsva(attr.hue, attr.sat, attr.val * 0.7, 1) }
                            }

                        }

                        onTouchDown: {
                            controller.midiMapping().guiControlHasBeenTouched(extInputH.mappingID)
                            updateHue(touch)
                        }
                        onTouchMove: updateHue(touch)

                        function updateHue(touch) {
                            var angle = getAngle(touch)
                            attr.hue = 1 - ((angle / (2*Math.PI)) % 1)
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
                            target: attr
                            onValueChanged: touchArea.setCursorPosition(attr.hue)
                        }
                        Component.onCompleted: {
                            touchArea.setCursorPosition(attr.hue)
                        }
                        onHeightChanged: touchArea.setCursorPosition(attr.hue)

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
                        value: attr.sat
                        onValueChanged: {
                            if (value !== attr.sat) {
                                attr.sat = value
                            }
                        }
                        hue: attr.hue
                        midiMappingEnabled: false
                        onTouchDown: controller.midiMapping().guiControlHasBeenTouched(extInputS.mappingID)
                    }

                    BrightnessSlider {
                        width: 30*dp
                        value: attr.val
                        onValueChanged: {
                            if (value !== attr.val) {
                                attr.val = value
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
            attr.hue = externalInput
        }
        Connections {
            target: attr
            onValueChanged: controller.midiMapping().sendFeedback(extInputH.mappingID, attr.hue)
        }
    }
    Item {
        id: extInputS
        property string mappingID: root.mappingID + "S"
        property real externalInput: -1
        Component.onCompleted: controller.midiMapping().registerGuiControl(this, mappingID)
        Component.onDestruction: if (controller) controller.midiMapping().unregisterGuiControl(mappingID)
        onExternalInputChanged: {
            attr.sat = externalInput
        }
        Connections {
            target: attr
            onValueChanged: controller.midiMapping().sendFeedback(extInputS.mappingID, attr.sat)
        }
    }
    Item {
        id: extInputV
        property string mappingID: root.mappingID + "V"
        property real externalInput: -1
        Component.onCompleted: controller.midiMapping().registerGuiControl(this, mappingID)
        Component.onDestruction: if (controller) controller.midiMapping().unregisterGuiControl(mappingID)
        onExternalInputChanged: {
            attr.val = externalInput
        }
        Connections {
            target: attr
            onValueChanged: controller.midiMapping().sendFeedback(extInputV.mappingID, attr.val)
        }
    }
    Component.onCompleted: {
        controller.midiMapping().sendFeedback(extInputH.mappingID, attr.hue)
        controller.midiMapping().sendFeedback(extInputS.mappingID, attr.sat)
        controller.midiMapping().sendFeedback(extInputV.mappingID, attr.val)
    }
}
