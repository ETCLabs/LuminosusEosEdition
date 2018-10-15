import QtQuick 2.10
import QtGraphicalEffects 1.0
import CustomElements 1.0

Item {
    id: root

    property bool open: true

    property Item blurSource: parent
    property rect blurArea: Qt.rect(x, y, width, height)

    onOpenChanged: if (open) positionAnimation.enabled = true

    Loader {
        id: blur
        anchors.fill: parent
        active: GRAPHICAL_EFFECTS_LEVEL >= 2 && opacity > 0.0
        opacity: open ? 1.0 : 0.0
        sourceComponent: FastBlur {
            radius: 16*dp

            source: ShaderEffectSource {
                sourceItem: blurSource
                sourceRect: blurArea
            }
        }

        Behavior on opacity {
            NumberAnimation {
                duration: 600
                easing.type: Easing.OutCubic
            }
        }
    }

    Rectangle {
        anchors.fill: parent
        color: "black"
        opacity: open ? 0.4 : 0.0

        Behavior on opacity {
            OpacityAnimator {
                duration: 600
                easing.type: Easing.OutCubic
            }
        }
    }

    CustomTouchArea {
        anchors.fill: parent
        enabled: open
        onClick: {
            open = false
        }
        onScrollEvent: scrollEventWasAccepted()
    }

    RectangularGlow {
        anchors.fill: dialogBg
        glowRadius: 60*dp
        spread: 0.1
        color: Qt.hsva(0, 0, 0, 0.5)
    }

    Rectangle {
        id: dialogBg
        width: 600*dp
        height: 500*dp
        anchors.centerIn: parent
        anchors.verticalCenterOffset: open ? 0 : (root.height / 2 + height / 2 + 60*dp /*shadow*/)
        color: "#333"
        border.color: "#aaa"
        border.width: 1

        Behavior on anchors.verticalCenterOffset {
            id: positionAnimation
            enabled: true
            NumberAnimation {
                duration: 400
                easing.type: Easing.OutCubic
            }
        }

        Loader {
            anchors.fill: parent
            asynchronous: true
            active: true // open  // FIXME: does this slow down startup?
            source: "qrc:/qml/Tutorial/TutorialStackView.qml"
        }
    }
}
