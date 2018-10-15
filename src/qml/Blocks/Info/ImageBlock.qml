import QtQuick 2.5
import QtQuick.Dialogs 1.2
import CustomElements 1.0
import "../../CustomBasics"
import "../../CustomControls"

BlockBase {
    id: root
    width: 130*dp
    height: controller.anchorManager().presentationMode
        ? width * (image.implicitHeight / Math.max(image.implicitWidth, 1))
        : width * (image.implicitHeight / Math.max(image.implicitWidth, 1)) + 30*dp
    settingsComponent: settings

    // hide background in presentation mode:
    color: controller.anchorManager().presentationMode ? "transparent" : "#444"
    showShadows: !controller.anchorManager().presentationMode || block.shadow

    z: -1  // always in background

    StretchColumn {
        height: parent.height
        width: parent.width

        BlockRow {
            implicitHeight: -1  // stretch
            Image {
                id: image
                anchors.fill: parent
                source: block.filePath
                rotation: block.rotation

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
            text: "Image"
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
                        title: "Select Image File"
                        folder: shortcuts.pictures
                        selectMultiple: false
                        nameFilters: "Image Files (*)"
                        onAccepted: {
                            if (fileUrl) {
                                block.filePath = fileUrl
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

            BlockRow {
                StretchText {
                    text: "Rotation:"
                }
                NumericInput {
                    width: 60*dp
                    implicitWidth: 0  // do not stretch
                    minimumValue: 0
                    maximumValue: 360
                    value: block.rotation
                    onValueChanged: {
                        if (value !== block.rotation) {
                            block.rotation = value
                        }
                    }
                }
            }

            BlockRow {
                Text {
                    text: "Shadow:"
                    width: parent.width - 30*dp
                }
                CheckBox {
                    width: 30*dp
                    active: block.shadow
                    onActiveChanged: {
                        if (active !== block.shadow) {
                            block.shadow = active
                        }
                    }
                }
            }
        }
    }  // end Settings Component
}

