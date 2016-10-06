import QtQuick 2.5
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0
import QtQuick.Controls 1.4
import CustomGeometry 1.0
import CustomElements 1.0
import "CustomControls"
import "CustomBasics"
import "Blocks"
import "."


Window {
    title: "luminosus - " + controller.getVersionString() + " (Beta)"
    width: 800
    height: 600
    property bool bgIsSeamless: controller.backgroundName.indexOf("tiled") !== -1
    visible: false  // is shown by MainController ctor

    Item {
        id: content
        anchors.fill: parent
        layer.enabled: GRAPHICAL_EFFECTS_LEVEL >= 3
        layer.smooth: false
        layer.mipmap: false

        Loader {
            sourceComponent: bgIsSeamless ? seamlessBackground : staticBackground
        }

        Component {
            id: seamlessBackground
            Image {
                width: content.width + sourceSize.width
                height: content.height + sourceSize.height
                fillMode: Image.Tile
                source: "qrc:/images/bg/" + controller.backgroundName
                x: ((plane.x % sourceSize.width) + sourceSize.width) % sourceSize.width - sourceSize.width
                y: ((plane.y % sourceSize.height) + sourceSize.height) % sourceSize.height - sourceSize.height
            }
        }
        Component {
            id: staticBackground
            Image {
                width: content.width
                height: content.height
                // using jpg instead of png to force opaque rendering
                source: "qrc:/images/bg/" + controller.backgroundName
                fillMode: Image.PreserveAspectCrop
            }
        }

        WorkspacePlaneTouchController {
            plane: plane
            anchors.fill: parent
            objectName: "planeController"
        }

        WorkspacePlane {
            // does not have a "size", because it is not visible by itself
            id: plane
            width: parent.width
            height: parent.height
            objectName: "plane"
            onXChanged: controller.blockManager().updateBlockVisibility(plane)
            onYChanged: controller.blockManager().updateBlockVisibility(plane)
        }

        // handle CTRL + Z and CTRL + V shortcut:
        Keys.onPressed: {
            if (event.key === Qt.Key_Z && (event.modifiers & Qt.ControlModifier)) {
                controller.blockManager().restoreDeletedBlock()
                event.accepted = true
            } else if (event.key === Qt.Key_V && (event.modifiers & Qt.ControlModifier)) {
                controller.blockManager().pasteBlock();
                event.accepted = true
            }
        }
    }

    // -------------------------------- Block Buttons ---------------------------------

    DarkBackgroundBlur {
        anchors.fill: blockButtonsRow
        blurSource: content
        visible: !controller.anchorManager().presentationMode
    }

    Rectangle {
        id: blockButtonsRow
        color: "transparent"
        width: 120*dp
        height: 40*dp
        x: 40*dp
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 40*dp
        border.width: 1*dp
        border.color: "#aaa"
        visible: !controller.anchorManager().presentationMode

        BorderImage {
            anchors.fill: parent
            anchors.margins: -16
            border { left: 16; top: 16; right: 16; bottom: 16 }
            source: "qrc:/images/shadow_border.png"
            visible: GRAPHICAL_EFFECTS_LEVEL >=2
        }

        StretchRow {
            anchors.fill: parent
            SvgButton {
                uid: "duplicate"
                implicitWidth: -1
                onPress: controller.blockManager().duplicateFocusedBlock()
                iconName: "copy"
                size: 28*dp
            }
            SvgButton {
                uid: "undo"
                implicitWidth: -1
                onPress: controller.blockManager().restoreDeletedBlock()
                iconName: "undo"
                size: 26*dp
            }
            SvgButton {
                objectName: "trash"
                uid: "trash"
                implicitWidth: -1
                onPress: controller.blockManager().deleteFocusedBlock()
                iconName: "trash"
                size: 30*dp
            }
        }
    }

    // -------------------------------- Presentation Buttons ---------------------------------

    DarkBackgroundBlur {
        anchors.fill: presentationButtonsRow
        blurSource: content
        visible: controller.anchorManager().anchorsExist
    }

    Rectangle {
        id: presentationButtonsRow
        color: "transparent"
        width: 120*dp
        height: 30*dp
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0*dp
        border.width: 1*dp
        border.color: "#aaa"
        visible: controller.anchorManager().anchorsExist

        BorderImage {
            anchors.fill: parent
            anchors.margins: -16
            border { left: 16; top: 16; right: 16; bottom: 16 }
            source: "qrc:/images/shadow_border.png"
            visible: GRAPHICAL_EFFECTS_LEVEL >=2
        }

        StretchRow {
            anchors.fill: parent
            SvgButton {
                implicitWidth: -1
                onPress: controller.anchorManager().showPrevious()
                iconName: "down_arrow_slim"
                rotation: 90
            }
            ButtonBottomLine {
                implicitWidth: -1
                onPress: controller.anchorManager().showCurrent()
                text: controller.anchorManager().currentIndex + 1
            }
            ButtonBottomLine {
                implicitWidth: -1
                onPress: controller.anchorManager().togglePresentationMode()
                text: controller.anchorManager().presentationMode ? "X" : "P"
            }
            SvgButton {
                implicitWidth: -1
                onPress: controller.anchorManager().showNext()
                iconName: "down_arrow_slim"
                rotation: -90
            }
        }
    }

    // -------------------------------- Drawer Button ---------------------------------

    DarkBackgroundBlur {
        anchors.fill: drawerButton
        blurSource: content
        visible: !controller.anchorManager().presentationMode
    }

    Rectangle {
        id: drawerButton
        width: 40*dp
        height: 40*dp
        anchors.right: drawer.left
        anchors.rightMargin: -1*dp
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 100*dp
        color: "transparent"
        border.width: 1*dp
        border.color: "#aaa"
        visible: !controller.anchorManager().presentationMode

        BorderImage {
            anchors.fill: parent
            anchors.margins: -16
            border { left: 16; top: 16; right: 16; bottom: 16 }
            source: "qrc:/images/shadow_border.png"
            visible: GRAPHICAL_EFFECTS_LEVEL >=2
        }

        Image {
            id: icon
            anchors.fill: parent
            anchors.margins: 8*dp

            source: "qrc:/images/svg/down_arrow_slim.svg"
            fillMode: Image.PreserveAspectFit
            smooth: true

            rotation: drawer.offset >= drawer.width ? 270 : 90
            Behavior on rotation {
                RotationAnimator {
                    duration: 600
                    easing.type: Easing.InCubic
                }
            }
        }

        CustomTouchArea {
            property real initialOffset: 0
            property real initialTouchX: 0
            anchors.fill: parent
            onTouchDown: {
                drawerOpenAnimation.stop()
                drawerCloseAnimation.stop()
                initialOffset = drawer.offset
            }
            onTouchMove: {
                drawer.offset = Math.max(0, Math.min(drawer.width, initialOffset + (touch.originX - touch.x)))
            }
            onTouchUp: {
                if (drawer.offset !== drawer.width && drawer.offset !== 0) {
                    if (drawer.offset > initialOffset) {
                        drawerOpenAnimation.start()
                    } else {
                        drawerCloseAnimation.start()
                    }
                } else if (drawer.offset - 3*dp <= initialOffset && drawer.offset + 3*dp >= initialOffset) {
                    if (drawer.offset < drawer.width / 2) {
                        drawerOpenAnimation.start()
                    } else {
                        drawerCloseAnimation.start()
                    }
                }
            }

            mouseOverEnabled: true
            Rectangle {
                id: mouseOverFeedback
                anchors.fill: parent
                color: "white"
                opacity: 0.05
                visible: parent.mouseOver
            }
        }
    }

    // -------------------------------- Drawer ---------------------------------

    DarkBackgroundBlur {
        anchors.fill: drawer
        blurSource: content
    }

    DrawerContent {
        id: drawer
        property real offset: 0
        visible: offset > 0
        width: 270*dp
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: parent.right
        anchors.leftMargin: -offset

        NumberAnimation {
            id: drawerCloseAnimation
            target: drawer
            property: "offset"
            duration: 500
            easing.type: Easing.OutBounce
            easing.amplitude: 0.2
            to: 0
        }

        NumberAnimation {
            id: drawerOpenAnimation
            target: drawer
            property: "offset"
            duration: 400
            easing.type: Easing.OutCubic
            to: drawer.width
        }

        Connections {
            target: controller.anchorManager()
            onPresentationModeChanged: {
                if (controller.anchorManager().presentationMode
                        && drawer.offset > 0) {
                    drawerCloseAnimation.start()
                }
            }
        }
    }

    Connections {
        target: controller
        onOpenBlockSettings: {
            if (!drawer.visible) {
                drawerCloseAnimation.stop()
                drawerOpenAnimation.start()
            }
        }
    }

    // -------------------------------- Toast Message ---------------------------------

    Rectangle {
        property bool toastIsWarning: false
        id: toast
        objectName: "toast"
        width: Math.max(toastText.implicitWidth + 20*dp, 100*dp)
        height: Math.max(toastText.implicitHeight + 20*dp, 30*dp)
        color: toastIsWarning ? Qt.rgba(0.15, 0, 0, 0.8) : Qt.rgba(0, 0, 0, 0.8)
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 60*dp
        opacity: 0
        Text {
            id: toastText
            text: "Test toast"
            anchors.centerIn: parent
            horizontalAlignment: Text.AlignHCenter
        }
        OpacityAnimator on opacity {
            id: toastHideAnimation
            from: 1
            to: 0
            running: false
            duration: 300
        }

        Timer {
            id: hideTimer
            interval: 4000
            onTriggered: toastHideAnimation.start()
        }
        function displayToast(text, isWarning) {
            toastText.text = text
            toast.opacity = 1
            toastIsWarning = isWarning
            hideTimer.start()
        }
    }



}
