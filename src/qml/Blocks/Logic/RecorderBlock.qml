import QtQuick 2.5
import CustomElements 1.0
import CustomStyle 1.0
import "../../CustomBasics"
import "../../CustomControls"

BlockBase {
    id: root
    width: 180*dp
    height: 180*dp

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
            InputNode {
                node: block.node("record")
            }
            SvgButton {
                width: 30*dp
                iconName: "record_fancy"
                onPress: block.attr("recording").val ? block.stopRecording() : block.startRecording()
            }
            StretchText {
                implicitWidth: -0.9
                hAlign: Text.AlignHCenter
                text: block.duration.toFixed(1) + "s"
            }
            StretchText {
                hAlign: Text.AlignHCenter
                text: block.bpm.toFixed(0) + " BPM"
            }
        }

        BlockRow {
            implicitHeight: -2  // -2 because two rows
            StretchColumn {
                width: 45*dp
                BlockRow {
                    InputNode {
                        node: block.node("play")
                    }
                    CenteredSvgImage {
                        width: 15*dp
                        iconName: "play"
                        color: "#999"
                    }
                }
                BlockRow {
                    InputNode {
                        node: block.node("pause")
                    }
                    CenteredSvgImage {
                        width: 15*dp
                        iconName: "pause"
                        color: "#999"
                    }
                }
            }
            SvgButton {
                implicitWidth: -1
                iconName: "pause"
                inverted: true
                onPress: block.stopPlayback()
                mappingID: block.getUid() + iconName
            }
            SvgButton {
                implicitWidth: -1
                iconName: "play"
                onPress: block.startPlayback()
                mappingID: block.getUid() + iconName
            }
        }

        BlockRow {
            InputNode {
                node: block.node("toggle")
            }
            CenteredSvgImage {
                width: 15*dp
                iconName: "play_pause"
                color: "#999"
            }
            ButtonBottomLine {
                text: "Sync"
                onPress: block.sync()
                mappingID: block.getUid() + text
            }
        }

        DragArea {
            text: "Recorder"
            OutputNode {
                node: block.node("link")
            }
        }
    }
}

