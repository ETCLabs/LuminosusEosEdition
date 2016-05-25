import QtQuick 2.5
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0
import QtQuick.Controls 1.4
import CustomGeometry 1.0
import CustomElements 1.0
import "qml/CustomControls"
import "qml/CustomBasics"
import "qml/Blocks"
import "qml/"


Window {
	title: "luminosus - " + controller.getVersionString() + " (Pre-Alpha)"
    width: 800
    height: 600
	property bool seamlessBackground: false

    Item {
        id: content
        anchors.fill: parent
		layer.enabled: GRAPHICAL_EFFECTS_LEVEL >= 3
		layer.smooth: false
		layer.mipmap: false

//        Image {
//            id: seamlessBg
//            width: parent.width + sourceSize.width
//            height: parent.height + sourceSize.height
//            fillMode: Image.Tile
//            source: "qrc:/images/bg/polygon_grey.png"
//            x: ((plane.offsetX % sourceSize.width) + sourceSize.width) % sourceSize.width - sourceSize.width
//            y: ((plane.offsetY % sourceSize.height) + sourceSize.height) % sourceSize.height - sourceSize.height
//            visible: seamlessBackground
//        }

        Image {
            id: staticBg
            anchors.fill: parent
            source: Screen.primaryOrientation === Qt.LandscapeOrientation ? "qrc:/images/bg/polygon_grey.png" : "qrc:/images/bg/polygon_grey_portrait.png"
            visible: !seamlessBackground
            fillMode: Image.PreserveAspectCrop
        }

        InfinitePlaneTouchController {
            plane: plane
            anchors.fill: parent
			objectName: "planeController"
        }

        InfinitePlane {
            // does not have a "size", because it is not visible by itself
            id: plane
            objectName: "plane"
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

	DarkBackgroundBlur {
		anchors.fill: blockButtonsRow
		blurSource: content
		//brightness: -0.4
		//alternativeColor: "#666"
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

	DarkBackgroundBlur {
		anchors.fill: drawerButton
		blurSource: content
		//brightness: -0.4
		//alternativeColor: "#666"
	}

    Rectangle {
        id: drawerButton
        width: 40*dp
        height: 40*dp
		anchors.right: drawer.left
		anchors.rightMargin: -1*dp
		anchors.bottom: parent.bottom
		anchors.bottomMargin: 80*dp
		color: "transparent"
        border.width: 1*dp
        border.color: "#aaa"
        BorderImage {
            anchors.fill: parent
            anchors.margins: -16
            border { left: 16; top: 16; right: 16; bottom: 16 }
            source: "qrc:/images/shadow_border.png"
			visible: GRAPHICAL_EFFECTS_LEVEL >=2
        }
		Text {
			font.family: "breeze-icons"
			text: ""
			font.pixelSize: 28*dp
			color: "#fff"
			anchors.centerIn: parent
		}

//        Image {
//            id: icon
//            source: "qrc:/images/plus_icon.png"
//            anchors.fill: parent
//            anchors.margins: 8*dp
//            visible: false
//            smooth: true
//        }
//        ColorOverlay {
//            anchors.fill: icon
//            source: icon
//            color: "#000"
//        }
        TouchArea {
            property real initialOffset: 0
            property real initialTouchX: 0
            anchors.fill: parent
            onTouchDown: {
                drawerOpenAnimation.stop()
                drawerCloseAnimation.stop()
                initialOffset = drawer.offset
                initialTouchX = touch.sceneX
            }
            onTouchMove: {
                drawer.offset = Math.max(0, Math.min(drawer.width, initialOffset + (initialTouchX - touch.sceneX)))
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
        }
	}

	DarkBackgroundBlur {
		anchors.fill: drawer
		blurSource: content
	}

    DrawerContent {
        id: drawer
		property real offset: 0
        visible: offset > 0
        width: 240*dp
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
            duration: 500
            easing.type: Easing.OutBounce
            easing.amplitude: 0.8
            to: drawer.width
        }
    }

    Rectangle {
        id: toast
        width: 300*dp
        height: 60*dp
        color: Qt.rgba(0, 0, 0, 0.8)
        y: parent.height - 100*dp
        x: parent.width / 2 - width / 2
        opacity: 0
        Text {
            id: toastText
            text: "Test toast"
            anchors.centerIn: parent
        }
        Behavior on opacity {
            NumberAnimation {
                duration: 300
            }
        }

        Timer {
            id: hideTimer
            interval: 3000;
            onTriggered: toast.opacity = 0
        }
        function displayToast(text) {
            toastText.text = text
            toast.opacity = 1
            hideTimer.start()
        }
    }



}
