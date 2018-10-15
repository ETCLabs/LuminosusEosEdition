import QtQuick 2.5
import CustomElements 1.0

import "../../CustomBasics"
import "../../CustomControls"


BlockBase {
	id: root
    width: open ? 200*dp : 140*dp
    height: open ? 160*dp : 30*dp

    property bool open: false

	StretchColumn {
		anchors.fill: parent

        SpectrumItem {
            visible: root.open
            implicitHeight: -1  // stretch
            points: block.valueHistory
            lineWidth: 1*dp
            color: "#888"

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                anchors.topMargin: 10*dp
                font.family: "Courier"
                horizontalAlignment: Text.AlignRight
                text: block.attr("additionalValueString").val
            }
        }

        DragArea {
            InputNode {
                node: block.node("inputNode")
            }

            Text {
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter
                font.family: "Courier"
                text: block.valueString
                color: block.attr("currentValue").val <= 0.0 ? "red" : (block.attr("currentValue").val < 1.0 ? "orange" : "lightgreen")
            }

            Item {  // arrow
                width: 30*dp
                height: 30*dp
                anchors.right: parent.right
                Image {
                    width: 10*dp
                    height: 5*dp
                    anchors.centerIn: parent
                    source: "qrc:/images/down_arrow_slim.png"
                    rotation: root.open ? 180 : 0
                    Behavior on rotation {
                        NumberAnimation {
                            duration: 300
                            easing.type: Easing.OutCubic
                        }
                    }
                }
                CustomTouchArea {
                    anchors.fill: parent
                    onClick: {
                        root.open = !root.open
                        root.y = open ? root.y-130*dp : root.y+130*dp
                        root.x = open ? root.x-60*dp : root.x+60*dp
                        block.positionChangedExternal()
                    }
                }
            }
		}
	}
}

