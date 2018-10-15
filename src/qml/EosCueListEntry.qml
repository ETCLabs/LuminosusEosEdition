import QtQuick 2.5
import CustomElements 1.0
import "CustomBasics"
import "CustomControls"

Rectangle {
    height: (notesVisible ? 60*dp : 30*dp) + (modelData.scene ? 30*dp : 0)
    color: modelData.isActive ? Qt.rgba(0.8, 0.65, 0.3, 0.5)
                           : ((modelData.isPending) ? Qt.rgba(1, 1, 1, 0.1) : "transparent")
    anchors.left: parent.left
    anchors.right: parent.right

    property bool notesVisible: false
    property bool editMode: false

    Loader {
        id: sceneNameLoader
        height: 30*dp
        anchors.left: parent.left
        anchors.right: parent.right
        sourceComponent: sceneNameComponent
        active: modelData.scene
    }
    Loader {
        height: 30*dp
        y: sceneNameLoader.active ? 30*dp : 0
        anchors.left: parent.left
        anchors.right: parent.right
        sourceComponent: viewModeView
        active: !editMode
    }
    Loader {
        height: 30*dp
        y: sceneNameLoader.active ? 30*dp : 0
        anchors.left: parent.left
        anchors.right: parent.right
        sourceComponent: editModeView
        active: editMode
    }

    // ---------------------------- Scene Name ------------------------------
    Component {
        id: sceneNameComponent
        StretchRow {
            Item {  // green line next to scene name
                implicitWidth: -1

                Rectangle {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.leftMargin: 10*dp
                    anchors.rightMargin: 10*dp
                    anchors.verticalCenter: parent.verticalCenter
                    height: 4*dp
                    color: "#080"
                }
            }
            Text {
                id: sceneNameText
                text: modelData.scene
                width: implicitWidth
            }
            Item {  // green line next to scene name
                implicitWidth: -1

                Rectangle {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.leftMargin: 10*dp
                    anchors.rightMargin: 10*dp
                    anchors.verticalCenter: parent.verticalCenter
                    height: 4*dp
                    color: "#080"
                }
            }
        }
    }

    // ---------------------------- View Only ------------------------------
    Component {
        id: viewModeView
        BlockRow {
            // size set by loader

            Item {  // arrow
                width: 30*dp
                Image {
                    width: 10*dp
                    height: 5*dp
                    anchors.centerIn: parent
                    source: modelData.notes ? "qrc:/images/down_arrow_slim_blue.png" : "qrc:/images/down_arrow_slim.png"
                    rotation: notesVisible ? 0 : -90
                    Behavior on rotation {
                        NumberAnimation {
                            duration: 300
                            easing.type: Easing.OutCubic
                        }
                    }
                }

                CustomTouchArea {
                    anchors.fill: parent
                    onClick: notesVisible = !notesVisible
                }
            }
            Text {
                width: 90*dp
                font.bold: true
                text: modelData.cueNumber
                color: "#fff"
            }
            Item {
                implicitWidth: -1
                Text {
                    anchors.fill: parent
                    text: modelData.label
                    color: "#fff"
                    elide: Text.ElideRight
                }
            }
            Text {
                width: 20*dp
                text: "+"
                color: Qt.rgba(0.3, 0.6, 1, 1)
                CustomTouchArea {
                    anchors.fill: parent
                    onClick: notesVisible = !notesVisible
                }
                visible: modelData.notes
            }
            Text {
                width: 60*dp
                text: modelData.upTimeDuration < 0 ?
                          "-"
                        : (modelData.upTimeDuration).toFixed(1)
                horizontalAlignment: Text.AlignHCenter
            }
            Text {
                width: 80*dp
                text: modelData.downTimeDuration < 0 ?
                          "-"
                        : (modelData.downTimeDuration).toFixed(1)
                horizontalAlignment: Text.AlignHCenter
            }

            IconButton {
                width: 30*dp
                iconName: "edit_icon"
                onClick: editMode = true
            }
            IconButton {
                width: 30*dp
                iconName: "fader_go_icon"
                onClick: modelData.fire()
            }
            ButtonBottomLine {
                implicitWidth: 0
                width: 40*dp
                text: "Use"
                onClick: modelData.createCueBlock()
            }
        }  // end view only Row
    }


    // ---------------------------- Edit View ------------------------------

    Component {
        id: editModeView

        BlockRow {
            // size set by loader

            Item {  // arrow
                width: 30*dp
                Image {
                    width: 10*dp
                    height: 5*dp
                    anchors.centerIn: parent
                    source: "qrc:/images/down_arrow_slim_blue.png"
                    rotation: notesVisible ? 0 : -90
                    Behavior on rotation {
                        NumberAnimation {
                            duration: 300
                            easing.type: Easing.OutCubic
                        }
                    }
                }

                CustomTouchArea {
                    anchors.fill: parent
                    onClick: notesVisible = !notesVisible
                }
            }
            TextInput {
                id: cueNumberInput
                width: 90*dp
                font.bold: true
                text: modelData.cueNumber
                color: "#f33"
                Keys.onTabPressed: labelInput.forceActiveFocus()
            }
            Item {
                implicitWidth: -1
                TextInput {
                    id: labelInput
                    anchors.fill: parent
                    clip: true
                    text: modelData.label
                    color: "#f33"
                    onAccepted: save()
                    Keys.onTabPressed: upTimeInput.forceActiveFocus()
                    Keys.onBacktabPressed: cueNumberInput.forceActiveFocus()
                }
            }
            TextInput {
                id: upTimeInput
                width: 60*dp
                text: modelData.upTimeDuration < 0 ?
                          ""
                        : (modelData.upTimeDuration).toFixed(1)
                color: "#f33"
                horizontalAlignment: Text.AlignRight
                Keys.onTabPressed: downTimeInput.forceActiveFocus()
                Keys.onBacktabPressed: labelInput.forceActiveFocus()
            }
            TextInput {
                id: downTimeInput
                width: 80*dp
                text: modelData.downTimeDuration < 0 ?
                          ""
                        : (modelData.downTimeDuration).toFixed(1)
                color: "#f33"
                horizontalAlignment: Text.AlignRight
                Keys.onBacktabPressed: upTimeInput.forceActiveFocus()
            }
            IconButton {
                width: 30*dp
                iconName: "tick_icon_green"
                onClick: save()
            }
            IconButton {
                width: 30*dp
                iconName: "cancel_icon_red"
                onClick: editMode = false
            }
            IconButton {
                width: 40*dp
                iconName: "trash_icon"
                clickDurationEnabled: true
                onShortClick: guiManager.showToast("Long Click to Delete Cue")
                onLongClick: {
                    guiManager.showToast("Deleting Cue...")
                    modelData.deleteCue()
                }
            }

            function save() {
                editMode = false
                if (labelInput.displayText !== modelData.label) {
                    modelData.label = labelInput.displayText
                }
                var upTime = parseFloat(upTimeInput.text || -1)
                var downTime = parseFloat(downTimeInput.text || -1)
                modelData.upTimeDuration = upTime
                modelData.downTimeDuration = downTime
                // change Cue number at last because otherwise other commands don't work
                if (cueNumberInput.displayText !== modelData.cueNumber) {
                    modelData.setCueNumberByString(cueNumberInput.displayText)
                }
            }
        }  // end edit view Row
    }

    // ---------------------------- Notes ------------------------------

    Loader {
        height: 30*dp
        y: sceneNameLoader.active ? 60*dp : 30*dp
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: 30*dp
        sourceComponent: notesComponent
        active: notesVisible
    }

    Component {
        id: notesComponent

        BlockRow {
            // size set by loader

            Item {
                implicitWidth: -1
                TextInput {
                    id: notesInput
                    anchors.fill: parent
                    clip: true
                    color: displayText === modelData.notes ? "#fff" : "#f33"
                    hintText: "Notes"
                    text: modelData.notes
                    onEditingFinished: {
                        if (notesInput.displayText !== modelData.notes) {
                            modelData.notes = notesInput.displayText
                        }
                    }
                }
            }

            IconButton {
                width: 30*dp
                iconName: "tick_icon_green"
                onClick: {
                    if (notesInput.displayText !== modelData.notes) {
                        modelData.notes = notesInput.displayText
                    }
                }
            }
        }
    }
}
