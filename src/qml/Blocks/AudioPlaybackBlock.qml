import QtQuick 2.5
import QtQuick.Controls 1.4  // for ProgressBar
import CustomElements 1.0
import "../CustomBasics"
import "../CustomControls"

BlockBase {
    id: root
    width: 580*dp
    height: 120*dp

    StretchColumn {
        anchors.fill: parent

        BlockRow {  // main row

            StretchColumn {  // nodes column
                width: 80*dp
                implicitWidth: 0  // do not stretch
                BlockRow {  // play node row
                    InputNode {
                        objectName: "playNode"
                    }
                    StretchText {
                        text: "Play"
                    }
                }
                BlockRow {  // pause node row
                    InputNode {
                        objectName: "pauseNode"
                    }
                    StretchText {
                        text: "Pause"
                    }
                }
                BlockRow {  // both node row
                    InputNode {
                        objectName: "toggleNode"
                    }
                    StretchText {
                        text: "Both"
                    }
                }
            }  // end nodes column

            StretchColumn {  // play button column
                width: 60*dp
                implicitWidth: 0  // do not stretch
                PlayPauseButton {
                    height: 60*dp
                    isPlaying: block.isPlaying
                    onPress: block.onToggleDown()
                    onTouchUp: block.onToggleUp()
                }
                ButtonBottomLine {
                    toggle: true
                    text: "Toggle"
                    active: block.toggleMode
                    onActiveChanged: {
                        if (active !== block.toggleMode) {
                            block.toggleMode = active
                        }
                    }
                }
            }  // end play button column

            StretchColumn {  // seek bar and options column
                implicitWidth: -1  // stretch
                leftMargin: 15*dp

                Item {  // seek bar
                    implicitWidth: -1  // stretch
                    height: 30*dp

                    Rectangle {
                        color: "#555"
                        anchors.verticalCenter: parent.verticalCenter
                        height: 4*dp
                        width: parent.width
                    }

                    Rectangle {
                        property real value: block.playbackPosition
                        id: seekBar
                        color: Qt.rgba(0.3, 0.6, 1, 1)
                        anchors.verticalCenter: parent.verticalCenter
                        height: 4*dp
                        width: parent.width * value
                    }

                    Rectangle {
                        height: 4*dp
                        width: Math.min(seekBar.width, 4*dp)
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.right: seekBar.right
                        color: "yellow"
                        opacity: block.isPlaying ? 1 : 0
                        Behavior on opacity {
                            OpacityAnimator {
                                duration: 300
                            }
                        }
                    }

                    CustomTouchArea {
                        anchors.fill: parent
                        onTouchDown: {
                            block.setPlaybackPosition(touch.itemX / width)
                        }
                        onTouchMove: {
                            block.setPlaybackPosition(touch.itemX / width)
                        }
                    }
                }

                BlockRow {  // position and lenght labels
                    Text {
                        id: positionLabel
                        width: 80*dp
                        text: block.positionString
                        color: "#aaa"
                        font.family: "Courier"
                    }
                    Item {
                        // placeholder
                        implicitWidth: -1  // stretch
                    }
                    Text {
                        id: lengthLabel
                        width: 80*dp
                        text: block.lengthString
                        color: "#aaa"
                        font.family: "Courier"
                    }
                }

                BlockRow {  // options row
                    Text {
                        width: 100*dp
                        text: "From Begin"
                        horizontalAlignment: Text.AlignRight
                        CustomTouchArea {
                            anchors.fill: parent
                            onClick: fromBeginCheckbox.active = !fromBeginCheckbox.active
                        }
                    }
                    CheckBox {
                        id: fromBeginCheckbox
                        width: 30*dp
                        active: block.alwaysStartAtBegin
                        onActiveChanged: {
                            if (active !== block.alwaysStartAtBegin) {
                                block.alwaysStartAtBegin = active
                            }
                        }
                    }
                    Text {
                        width: 60*dp
                        text: "Loop"
                        horizontalAlignment: Text.AlignRight
                        CustomTouchArea {
                            anchors.fill: parent
                            onClick: loopCheckbox.active = !loopCheckbox.active
                        }
                    }
                    CheckBox {
                        id: loopCheckbox
                        width: 30*dp
                        active: block.loop
                        onActiveChanged: {
                            if (active !== block.loop) {
                                block.loop = active
                            }
                        }
                    }
                    Item {
                        // placeholder
                        implicitWidth: -1  // stretch
                    }
                    ButtonBottomLine {
                        width: 120*dp
                        implicitWidth: 0 // do not stretch
                        text: "Choose File"
                        onPress: {
                            fileDialogLoader.source = ""
                            fileDialogLoader.source = "qrc:/qml/AudioPlaybackBlockFileSelector.qml"
                        }

                        Loader {
                            id: fileDialogLoader
                        }
                    }
                }  // end options row

            }  // end seek bar and options column

            StretchColumn {  // output nodes column
                width: 80*dp
                implicitWidth: 0  // do not stretch
                BlockRow {
                    StretchText {
                        text: "Active"
                        hAlign: Text.AlignRight
                    }
                    OutputNode {
                        objectName: "activeNode"
                    }
                }
                BlockRow {
                    StretchText {
                        text: "End"
                        hAlign: Text.AlignRight
                    }
                    OutputNode {
                        objectName: "endNode"
                    }
                }
                BlockRow {
                    StretchText {
                        text: "Pos."
                        hAlign: Text.AlignRight
                    }
                    OutputNode {
                        objectName: "positionNode"
                    }
                }
            }  // end output nodes column

        }  // end main row

        DragArea {
            text: block.fileBaseName || "No File Loaded"
        }
    }  // end main Column
}
