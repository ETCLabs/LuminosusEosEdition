import QtQuick 2.5
import CustomElements 1.0

import "../CustomControls"
import "../CustomBasics"

StretchColumn {
    id: root
    height: implicitHeight
    defaultSize: 30*dp

    property bool collapsed: true
    property alias name: label.text

    CustomTouchArea {
        id: row
        height: 40*dp

        onTouchUp: {
            if (!canceled) {
                collapsed = !collapsed
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

        StretchRow {  // row with left margin
            anchors.fill: parent
            leftMargin: 5*dp
            Item {  // arrow
                width: 30*dp
                Image {
                    width: 16*dp
                    height: 8*dp
                    anchors.centerIn: parent
                    source: "qrc:/images/down_arrow_slim.png"
                    rotation: collapsed ? -90: 0
                    Behavior on rotation {
                        NumberAnimation {
                            duration: 300
                            easing.type: Easing.OutCubic
                        }
                    }
                }
            }

            Text {
                id: label
                width: parent.width - 30*dp
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize: 22*dp
            }
        } // end row with left margin
    }
}
