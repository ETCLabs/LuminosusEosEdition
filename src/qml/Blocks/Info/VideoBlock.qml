import QtQuick 2.5
import QtQuick.Dialogs 1.2
import QtMultimedia 5.8
import CustomElements 1.0
import "../../CustomBasics"
import "../../CustomControls"

BlockBase {
    id: root
    width: 130*dp
    height: controller.anchorManager().presentationMode
        ? width * 0.5625
        : width * 0.5625 + 30*dp  // * 0.5625 means 16:9 aspect ratio
    settingsComponent: settings

    // hide background in presentation mode:
    //color: controller.anchorManager().presentationMode ? "transparent" : "#444"
    //showShadows: !controller.anchorManager().presentationMode

    z: -1  // always in background

    StretchColumn {
        height: parent.height
        width: parent.width

        BlockRow {
            implicitHeight: -1  // stretch
            Video {
                id: video
                anchors.fill: parent
                source: block.attr("filePath").val

                CustomTouchArea {
                    anchors.fill: parent
                    onClick: {
                        if (video.playbackState === MediaPlayer.PlayingState) {
                            video.stop()
                        } else {
                            video.play()
                        }
                    }
                }

                // --------------------- Position Help Lines ---------------

                Rectangle {  // left
                    height: 2000*dp
                    y: -1000*dp
                    width: 1*dp
                    x: 0*dp
                    color: Qt.rgba(0.3, 0.6, 1, 1)
                    visible: root.pressed
                }
                Rectangle {  // right
                    height: 2000*dp
                    y: -1000*dp
                    width: 1*dp
                    anchors.right: parent.right
                    color: Qt.rgba(0.3, 0.6, 1, 1)
                    visible: root.pressed
                }
                Rectangle {  // | center
                    height: 2000*dp
                    y: -1000*dp
                    width: 1*dp
                    anchors.horizontalCenter: parent.horizontalCenter
                    color: Qt.rgba(0.3, 0.6, 1, 0.5)
                    visible: root.pressed
                }
                Rectangle {  // - center
                    width: 2000*dp
                    x: -1000*dp
                    height: 1*dp
                    anchors.verticalCenter: parent.verticalCenter
                    color: Qt.rgba(0.3, 0.6, 1, 0.5)
                    visible: root.pressed
                }
                Rectangle {  // top
                    width: 2000*dp
                    x: -1000*dp
                    height: 1*dp
                    y: 0*dp
                    color: Qt.rgba(0.3, 0.6, 1, 1)
                    visible: root.pressed
                }
                Rectangle {  // bottom
                    width: 2000*dp
                    x: -1000*dp
                    height: 1*dp
                    anchors.bottom: parent.bottom
                    color: Qt.rgba(0.3, 0.6, 1, 1)
                    visible: root.pressed
                }
            }
        }

        DragArea {
            text: block.attr("filePath").val || "Video"
            visible: !controller.anchorManager().presentationMode

            WidthAndHeightResizeArea {
                target: root
                minSize: 60*dp
                maxSize: 2000*dp
            }
        }
    }

    // -------------------------- Settings ----------------------------

    Component {
        id: settings
        StretchColumn {
            leftMargin: 15*dp
            rightMargin: 15*dp
            defaultSize: 30*dp

            BlockRow {
                StretchText {
                    text: "File:"
                }
                ButtonBottomLine {
                    width: 60*dp
                    text: "Select"
                    onClick: fileDialogLoader.active = true
                }

                Loader {
                    id: fileDialogLoader
                    active: false

                    sourceComponent: FileDialog {
                        id: fileDialog
                        title: "Select Video File"
                        folder: shortcuts.movies
                        selectMultiple: false
                        nameFilters: "Video Files (*)"
                        onAccepted: {
                            if (fileUrl) {
                                block.attr("filePath").val = fileUrl
                            }
                            fileDialogLoader.active = false
                        }
                        onRejected: {
                            fileDialogLoader.active = false
                        }
                        Component.onCompleted: {
                            // don't set visible to true before component is complete
                            // because otherwise the dialog will not be configured correctly
                            visible = true
                        }
                    }
                }
            }
        }
    }  // end Settings Component
}

