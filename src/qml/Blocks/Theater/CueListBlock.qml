import QtQuick 2.5
import CustomElements 1.0
import CustomStyle 1.0
import "../../CustomBasics"
import "../../CustomControls"
import "../../."  // to import EosCueListEntry

BlockBase {
    id: root
    width: 540*dp
    height: 500*dp
    settingsComponent: settings

    StretchColumn {
        anchors.fill: parent

        Rectangle {
            height: 30*dp
            color: "#333"
            BlockRow {
                anchors.fill: parent
                Item {
                    width: 30*dp
                }
                Text {
                    width: 120*dp
                    text: "Preset"
                }
                StretchText {
                    text:  "Description"
                }
                Text {
                    width: 60*dp
                    text: "Fade In"
                }
                Item {
                    width: 40*dp
                }
                Text {
                    width: 60*dp
                    text: "Hold"
                    horizontalAlignment: Text.AlignHCenter
                }
                Item {
                    width: 120*dp
                }
                HeightResizeArea {
                    width: 30*dp
                    target: root
                    minSize: 300*dp
                    maxSize: 1000*dp
                }
            }
        }


        Item {
            id: cuesColumn
            implicitHeight: -1

            ListView {
                id: cueListView
                anchors.fill: parent
                model: block.cues
                clip: true
                // scroll to pending Cue:
                highlightFollowsCurrentItem: true
                currentIndex: model.indexOf(block.pendingCue)
                preferredHighlightBegin: (height / 2) - 15*dp
                preferredHighlightEnd: (height / 2) + 15*dp
                highlightRangeMode: ListView.ApplyRange

                property var cueToMove: false

                delegate: CueListEntry {
                    list: cueListView
                }
            }

            StretchText {
                anchors.fill: parent
                text: "Cue List is empty."
                hAlign: Text.AlignHCenter
                visible: cueListView.count === 0
            }
        }

        Item {
            height: 3*dp
            Rectangle {
                anchors.fill: parent
                color: "#555"
            }
            Rectangle {
                height: parent.height
                width: parent.width * block.attr("fadePos").val
                color: Style.primaryActionColor
            }
        }
        Item {
            height: 3*dp
            Rectangle {
                height: parent.height
                width: parent.width * block.attr("holdPos").val
                color: "#888"
            }
        }
        BlockRow {
            height: 60*dp
            implicitHeight: 0

            StretchColumn {
                implicitWidth: -1
                BlockRow {
                    ButtonSideLine {
                        width: 90*dp
                        implicitWidth: 0
                        text: "+ Cue"
                        onPress: block.createCueFromMix()
                        mappingID: block.getUid() + "add_from_mix"
                    }
                    Item {
                        implicitWidth: -1
                    }
                }
                BlockRow {
                    height: 30*dp
                    implicitHeight: 0
                    InputNode {
                        node: block.node("manualFade")
                    }
                    StretchText {
                        text: "<- Manual"
                    }
                    ButtonSideLine {
                        width: 120*dp
                        implicitWidth: 0
                        text: block.attr("running").val ? "Pause" : "Continue"
                        onPress: block.triggerRunning()
                        mappingID: block.getUid() + "trigger_running"
                    }
                }
            }

            PushButton {
                width: 60*dp
                implicitWidth: 0
                onPress: block.go()
                mappingID: block.getUid() + "go"
                showLed: false
                Image {
                    width: 18*dp
                    height: 18*dp
                    anchors.centerIn: parent
                    anchors.verticalCenterOffset: parent.pressed ? -2*dp : -4*dp
                    source: "qrc:/images/fader_go_icon.png"
                }
            }
        }


        DragArea {
            implicitWidth: -1
            text: "Cue List"

            InputNode {
                node: block.node("go")
            }

            WidthResizeArea {
                id: resizeArea
                target: root
                minSize: 520*dp
                maxSize: 1140*dp
            }
        }

    }  // end main Column


    Component {
        id: settings
        StretchColumn {
            leftMargin: 15*dp
            rightMargin: 15*dp
            defaultSize: 30*dp

            ButtonSideLine {
                implicitWidth: -1
                text: "Pause & Reset List"
                onPress: block.pauseAndClear()
                mappingID: block.getUid() + "clear_list"
            }
            ButtonSideLine {
                implicitWidth: -1
                text: "Clear Benches"
                onPress: block.clearBenches()
                mappingID: block.getUid() + "clear_benches"
            }
        }
    }  // end Settings Component
}
