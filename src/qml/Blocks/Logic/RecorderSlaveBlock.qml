import QtQuick 2.5
import CustomElements 1.0
import CustomStyle 1.0
import "../../CustomBasics"
import "../../CustomControls"

BlockBase {
    id: root
    width: 180*dp
    height: 90*dp

    StretchColumn {
        anchors.fill: parent

        BlockRow {
            SpectrumItem {
                anchors.fill: parent
                points: block.data
                lineWidth: 1*dp
                color: "#777"

                InputNode {
                    node: block.node("inputNode")
                }

                Rectangle {  // playback position marker
                    width: 1*dp
                    height: parent.height
                    x: parent.width * block.relativePosition
                    color: Style.primaryHighlightColor
                }

                OutputNode {
                    node: block.node("outputNode")
                }
            }
        }

        BlockRow {
            ButtonBottomLine {
                text: block.attr("waitingForRecord").val ? "wait..." : "Overdub"
                onPress: block.toggleOverdub()
            }
        }

        DragArea {
            text: "Recorder"
            InputNode {
                node: block.node("link")
            }
        }
    }
}

