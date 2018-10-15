import QtQuick 2.5
import CustomElements 1.0
import CustomStyle 1.0
import "CustomBasics"
import "CustomControls"

Rectangle {
    height: 30*dp
    color: false ? Qt.rgba(0.8, 0.65, 0.3, 0.5)
                           : (false ? Qt.rgba(1, 1, 1, 0.1) : "transparent")
    anchors.left: parent.left
    anchors.right: parent.right

    property bool editMode: false
    property Item list

    Loader {
        height: 30*dp
        anchors.left: parent.left
        anchors.right: parent.right
        sourceComponent: viewModeView
        active: !editMode
    }
    Loader {
        height: 30*dp
        anchors.left: parent.left
        anchors.right: parent.right
        sourceComponent: editModeView
        active: editMode
    }

    // ---------------------------- View Only ------------------------------
    Component {
        id: viewModeView
        BlockRow {
            // size set by loader

            CustomTouchArea {
                width: 30*dp
                onClick: block.makePending(modelData)

                Rectangle {  // last
                    anchors.verticalCenter: parent.verticalCenter
                    height: 3*dp
                    width: (modelData === block.activeCue) ?
                             (parent.width - 5*dp) * block.attr("fadePos").val
                             : (parent.width - 5*dp) * (1 - block.attr("fadePos").val)
                    color: Style.primaryActionColor
                    visible: (modelData === block.lastCue || modelData === block.activeCue)
                }
                Rectangle {
                    height: 6*dp
                    width: 6*dp
                    radius: 3*dp
                    antialiasing: false
                    anchors.centerIn: parent
                    color: Style.primaryActionColor
                    visible: modelData === block.pendingCue
                }
            }

            Text {
                width: 120*dp
                text: modelData.sceneBlock.attr("label").val
                color: "#fff"
                elide: Text.ElideRight
                visible: modelData.sceneBlock.attr("label").val
            }
            Text {
                width: 120*dp
                text: "Unnamed"
                color: "#888"
                visible: !modelData.sceneBlock.attr("label").val
            }
            Item {
                implicitWidth: -1
                Text {
                    anchors.fill: parent
                    text: modelData.description
                    color: "#fff"
                    elide: Text.ElideRight
                }
            }
            Text {
                width: 60*dp
                text: modelData.fadeIn
                horizontalAlignment: Text.AlignHCenter
            }
            Item {
                width: 40*dp
            }
            Text {
                width: 60*dp
                text: modelData.follow ? modelData.hold : "-"
                horizontalAlignment: Text.AlignHCenter
            }
            ButtonBottomLine {
                width: 60*dp
                implicitWidth: 0
                text: modelData === list.cueToMove ? "Cancel" : "Move"
                visible: !list.cueToMove || modelData === list.cueToMove
                onClick: {
                    if (modelData === list.cueToMove) {
                        list.cueToMove = false
                    } else {
                        list.cueToMove = modelData
                    }
                }
            }
            SvgButton {
                width: 30*dp
                size: 15*dp
                iconName: "up_arrow_slim"
                onPress: {
                    var cue = list.cueToMove
                    list.cueToMove = false
                    block.moveAbove(cue, modelData)
                }
                visible: list.cueToMove && modelData !== list.cueToMove
            }
            SvgButton {
                width: 30*dp
                size: 15*dp
                iconName: "down_arrow_slim"
                onPress: {
                    var cue = list.cueToMove
                    list.cueToMove = false
                    block.moveBelow(cue, modelData)
                }
                visible: list.cueToMove && modelData !== list.cueToMove
            }
            IconButton {
                width: 30*dp
                iconName: "trash_icon"
                clickDurationEnabled: true
                onShortClick: guiManager.showToast("Long Click to Delete Cue")
                onLongClick: {
                    block.removeCue(modelData)
                }
            }
            IconButton {
                width: 30*dp
                iconName: "edit_icon"
                onClick: editMode = true
            }
            SvgButton {
                width: 30*dp
                implicitWidth: 0
                iconName: modelData.sceneBlock.guiIsHidden ? "eye_crossed" : "eye"
                onClick: modelData.sceneBlock.guiIsHidden ? modelData.sceneBlock.unhideGui() : modelData.sceneBlock.hideGui()
            }
        }  // end view only Row
    }


    // ---------------------------- Edit View ------------------------------

    Component {
        id: editModeView

        BlockRow {
            // size set by loader
            CustomTouchArea {
                width: 30*dp
                onClick: block.makePending(modelData)

                Rectangle {  // last
                    anchors.verticalCenter: parent.verticalCenter
                    height: 3*dp
                    width: (modelData === block.activeCue) ?
                             (parent.width - 5*dp) * block.attr("fadePos").val
                             : (parent.width - 5*dp) * (1 - block.attr("fadePos").val)
                    color: Style.primaryActionColor
                    visible: (modelData === block.lastCue || modelData === block.activeCue)
                }
                Rectangle {
                    height: 6*dp
                    width: 6*dp
                    radius: 3*dp
                    antialiasing: false
                    anchors.centerIn: parent
                    color: Style.primaryActionColor
                    visible: modelData === block.pendingCue
                }
            }
            TextInput {
                width: 120*dp
                clip: true
                text: modelData.sceneBlock.attr("label").val
                onDisplayTextChanged: {
                    if (displayText !== modelData.sceneBlock.attr("label").val) {
                        modelData.sceneBlock.attr("label").val = displayText
                    }
                }
            }
            Item {
                implicitWidth: -1
                TextInput {
                    anchors.fill: parent
                    clip: true
                    text: modelData.description
                    onDisplayTextChanged: {
                        if (displayText !== modelData.description) {
                            modelData.description = displayText
                        }
                    }
                }
            }
            NumericInput {
                width: 60*dp
                implicitWidth: 0
                value: modelData.fadeIn
                onValueChanged: {
                    if (value !== modelData.fadeIn) {
                        modelData.fadeIn = value
                    }
                }
                decimals: 1
            }
            CheckBox {
                width: 40*dp
                active: modelData.follow
                onActiveChanged: {
                    if (active !== modelData.follow) {
                        modelData.follow = active
                    }
                }
            }
            NumericInput {
                width: 60*dp
                implicitWidth: 0
                value: modelData.hold
                onValueChanged: {
                    if (value !== modelData.hold) {
                        modelData.hold = value
                    }
                }
                decimals: 1
                enabled: modelData.follow
            }
            ButtonBottomLine {
                width: 60*dp
                implicitWidth: 0
                text: modelData === list.cueToMove ? "Cancel" : "Move"
                visible: !list.cueToMove || modelData === list.cueToMove
                onClick: {
                    if (modelData === list.cueToMove) {
                        list.cueToMove = false
                    } else {
                        list.cueToMove = modelData
                    }
                }
            }
            SvgButton {
                width: 30*dp
                size: 15*dp
                iconName: "up_arrow_slim"
                onPress: {
                    var cue = list.cueToMove
                    list.cueToMove = false
                    block.moveAbove(cue, modelData)
                }
                visible: list.cueToMove && modelData !== list.cueToMove
            }
            SvgButton {
                width: 30*dp
                size: 15*dp
                iconName: "down_arrow_slim"
                onPress: {
                    var cue = list.cueToMove
                    list.cueToMove = false
                    block.moveBelow(cue, modelData)
                }
                visible: list.cueToMove && modelData !== list.cueToMove
            }
            IconButton {
                width: 30*dp
                iconName: "trash_icon"
                clickDurationEnabled: true
                onShortClick: guiManager.showToast("Long Click to Delete Cue")
                onLongClick: {
                    guiManager.showToast("Deleting Cue...")
                    block.removeCue(modelData)
                }
            }
            IconButton {
                width: 30*dp
                iconName: "tick_icon_green"
                onClick: editMode = false
            }
            SvgButton {
                width: 30*dp
                implicitWidth: 0
                iconName: modelData.sceneBlock.guiIsHidden ? "eye_crossed" : "eye"
                onClick: modelData.sceneBlock.guiIsHidden ? modelData.sceneBlock.unhideGui() : modelData.sceneBlock.hideGui()
            }
        }  // end edit view Row
    }
}
