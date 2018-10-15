import QtQuick 2.10
import QtGraphicalEffects 1.0
import CustomElements 1.0
import "../CustomBasics"
import "../CustomControls"

Item {

    property var tutorial

    Rectangle {
        anchors.fill: listView
        color: "#222"
    }

    ListView {
        id: listView
        anchors.fill: parent
        anchors.bottomMargin: 120*dp
        anchors.topMargin: 1
        anchors.leftMargin: 1
        anchors.rightMargin: 1
        clip: true  // for stack view animation

        model: tutorial.slides
        snapMode: ListView.SnapOneItem  // <- only move one item per swipe and then snap
        orientation: Qt.Horizontal
        highlightMoveVelocity: 1200*dp  // px/s -> needs to be scaled with dp
        // set currentIndex when list is moved by swipe:
        highlightRangeMode: ListView.StrictlyEnforceRange

        delegate: Image {
            width: listView.width
            height: listView.height
            source: "qrc:/images/tutorial/" + tutorial.folder + "/" + modelData.img + ".jpg"
            sourceSize.width: width
            sourceSize.height: height
            fillMode: Image.PreserveAspectFit
        }

        CustomTouchArea {
            anchors.fill: parent
            onClick: {
                var i = listView.currentIndex
                var newIndex = touch.itemX < width / 2 ? i-1 : i+1

                if (newIndex >= 0 && newIndex < tutorial.slides.length) {
                    listView.currentIndex = newIndex
                } else if (newIndex >= tutorial.slides.length) {
                    stackView.pop()
                }
            }
        }

        RectangularGlow {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.bottom
            height: 5
            color: "black"
            glowRadius: 20*dp
            spread: 0.2
            opacity: 0.5
        }

        Image {
            visible: listView.currentIndex > 0
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 20*dp
            width: 20*dp
            height: 10*dp
            source: "qrc:/images/down_arrow_slim.png"
            rotation: 90
        }
        Image {
            visible: listView.currentIndex < (tutorial.slides.length - 1)
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 20*dp
            width: 20*dp
            height: 10*dp
            source: "qrc:/images/down_arrow_slim.png"
            rotation: -90
        }
        Image {
            visible: listView.currentIndex === (tutorial.slides.length - 1)
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 20*dp
            width: 20*dp
            height: 20*dp
            source: "qrc:/images/tick_icon.png"
        }
    }

    Text {
        anchors.left: parent.left
        anchors.leftMargin: 50*dp
        anchors.right: parent.right
        anchors.rightMargin: 50*dp
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 28*dp
        height: 100*dp
        text: tutorial.slides[listView.currentIndex].text
        textFormat: Text.AutoText
        wrapMode: Text.Wrap
    }

    StretchRow {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10*dp
        width: parent.width * 0.4
        height: 8*dp
        Repeater {
            model: tutorial.slides.length
            Item {
                implicitWidth: -1
                Rectangle {
                    height: parent.height
                    width: height
                    anchors.centerIn: parent
                    radius: width / 2
                    color: modelData == listView.currentIndex ? "white" : "#555"
                }
            }
        }
    }
}
