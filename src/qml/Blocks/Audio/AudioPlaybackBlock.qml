import QtQuick 2.5
import QtQuick.Controls 1.4  // for ProgressBar
import CustomElements 1.0
import "../../CustomBasics"
import "../../CustomControls"

BlockBase {
    id: root
    width: 480*dp
    height: 120*dp
    settingsComponent: settings

    StretchColumn {
        anchors.fill: parent

        BlockRow {  // main row

            StretchColumn {  // nodes column
                width: 80*dp
                implicitWidth: 0  // do not stretch
                BlockRow {  // play node row
                    InputNode {
                        node: block.node("playNode")
                    }
                    StretchText {
                        text: "Play"
                    }
                }
                BlockRow {  // pause node row
                    InputNode {
                        node: block.node("pauseNode")
                    }
                    StretchText {
                        text: "Pause"
                    }
                }
                BlockRow {  // both node row
                    InputNode {
                        node: block.node("toggleNode")
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
                    mappingID: block.getUid() + "play_pause"
                    onExternalInputChanged: {
                        if (externalInput <= 0.) {
                            block.onToggleUp()
                        }
                    }
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
                    mappingID: block.getUid() + "toggle"
                }
            }  // end play button column

            StretchColumn {  // seek bar and options column
                implicitWidth: -1  // stretch
                leftMargin: 15*dp

                Item {  // seek bar
                    implicitHeight: -1  // stretch

                    Rectangle {  // seek bar background
                        color: "#555"
                        anchors.verticalCenter: parent.verticalCenter
                        height: 4*dp
                        width: parent.width
                        visible: !block.waveformIsAvailable
                    }

                    Rectangle {  // seek bar already played
                        property real value: block.playbackPosition
                        id: seekBar
                        color: block.waveformIsAvailable ? "#555" : Qt.rgba(0.3, 0.6, 1, 1)
                        anchors.verticalCenter: parent.verticalCenter
                        height: block.waveformIsAvailable ? parent.height : 4*dp
                        width: parent.width * value
                    }

                    Rectangle {  // seek bar cursor
                        height: block.waveformIsAvailable ? parent.height : 4*dp
                        width: Math.min(seekBar.width, block.waveformIsAvailable ? 1*dp : 4*dp)
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.right: seekBar.right
                        color: block.isPlaying ? "yellow" : Qt.rgba(0.3, 0.6, 1, 1)
                        Behavior on color {
                            ColorAnimation {
                                duration: 300
                            }
                        }
                    }

                    SpectrumItem {
                        anchors.fill: parent
                        points: block.waveform
                        lineWidth: 1*dp
                        color: "#aaa"
                        visible: block.waveformIsAvailable
                    }

                    Text {
                        id: positionLabel
                        width: 80*dp
                        anchors.top: parent.top
                        anchors.left: parent.left
                        text: block.positionString
                        color: "#aaa"
                        font.family: "Courier"
                    }
                    Text {
                        id: lengthLabel
                        width: 80*dp
                        anchors.top: parent.top
                        anchors.right: parent.right
                        text: block.lengthString
                        color: "#aaa"
                        font.family: "Courier"
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
                        node: block.node("activeNode")
                    }
                }
                BlockRow {
                    StretchText {
                        text: "End"
                        hAlign: Text.AlignRight
                    }
                    OutputNode {
                        node: block.node("endNode")
                    }
                }
                BlockRow {
                    StretchText {
                        text: "Pos."
                        hAlign: Text.AlignRight
                    }
                    OutputNode {
                        node: block.node("positionNode")
                    }
                }
            }  // end output nodes column

        }  // end main row

        DragArea {
            text: block.fileBaseName || "-> Choose File in Block Settings"

            WidthResizeArea {
                id: resizeArea
                target: root
                minSize: 420*dp
                maxSize: 1400*dp
            }
        }
    }  // end main Column

    Component {
        id: settings
        StretchColumn {
            leftMargin: 15*dp
            rightMargin: 15*dp
            defaultSize: 30*dp

            BlockRow {
                Text {
                    text: "Always From Begin:"
                    width: parent.width - 30*dp
                }
                CheckBox {
                    width: 30*dp
                    active: block.alwaysStartAtBegin
                    onActiveChanged: {
                        if (active !== block.alwaysStartAtBegin) {
                            block.alwaysStartAtBegin = active
                        }
                    }
                    mappingID: block.getUid() + "alwaysStartAtBegin"
                }
            }

            BlockRow {
                Text {
                    text: "Loop:"
                    width: parent.width - 30*dp
                }
                CheckBox {
                    width: 30*dp
                    active: block.loop
                    onActiveChanged: {
                        if (active !== block.loop) {
                            block.loop = active
                        }
                    }
                    mappingID: block.getUid() + "loop"
                }
            }

            ButtonBottomLine {
                height: 30*dp
                text: "Choose File"
                onPress: {
                    fileDialogLoader.source = ""
                    fileDialogLoader.source = "qrc:/qml/AudioPlaybackBlockFileSelector.qml"
                }
                mappingID: block.getUid() + "chooseFile"

                Loader {
                    id: fileDialogLoader
                }
            }
        }
    }  // end Settings component
}
