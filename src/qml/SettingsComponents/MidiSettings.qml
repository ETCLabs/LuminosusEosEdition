import QtQuick 2.5
import CustomElements 1.0

import "../CustomControls"
import "../CustomBasics"


StretchColumn {
    id: root
    leftMargin: 15*dp
    rightMargin: 15*dp
    defaultSize: 30*dp
    height: implicitHeight

    property string mappingID: "drawer"

    BlockRow {
        StretchText {
            text: "Devices:   " + controller.midi().inputNames.length
        }
        ButtonBottomLine {
            width: 90*dp
            implicitWidth: 0
            text: "Refresh"
            onPress: controller.midi().refreshDevices()
        }
    }

    Repeater {
        model: controller.midi().inputNames
        delegate: Item {
            height: 40*dp
            Rectangle {
                anchors.fill: parent
                anchors.bottomMargin: 10*dp
                color: "#333"
                StretchRow {
                    anchors.fill: parent
                    spacing: 7*dp
                    Rectangle {
                        width: 50*dp
                        color: "#555"
                        Text {
                            anchors.centerIn: parent
                            text: "In"
                            font.pixelSize: 14*dp
                            fontSizeMode: Text.Fit
                        }
                    }
                    StretchText {
                        text: modelData
                        elide: Text.ElideRight
                    }
                }  // StretchRow

                Rectangle {
                    anchors.fill: parent
                    color: "transparent"
                    border.width: 1
                    border.color: "lightgreen"
                }
            }  // Rectangle
        }  // delegate Item
    }

    Repeater {
        model: controller.midi().outputNames
        delegate: Item {
            height: 40*dp
            Rectangle {
                anchors.fill: parent
                anchors.bottomMargin: 10*dp
                color: "#333"
                StretchRow {
                    anchors.fill: parent
                    spacing: 7*dp
                    Rectangle {
                        width: 50*dp
                        color: "#555"
                        Text {
                            anchors.centerIn: parent
                            text: "Out"
                            font.pixelSize: 14*dp
                            fontSizeMode: Text.Fit
                        }
                    }
                    StretchText {
                        text: modelData
                        elide: Text.ElideRight
                    }
                }  // StretchRow

                Rectangle {
                    anchors.fill: parent
                    color: "transparent"
                    border.width: 1
                    border.color: "lightgreen"
                }
            }  // Rectangle
        }  // delegate Item
    }

    BlockRow {
        Text {
            text: "Omni Input Mode:"
            width: parent.width - 30*dp
            verticalAlignment: Text.AlignVCenter
        }
        CheckBox {
            id: omniModeCheckbox
            width: 30*dp
            active: controller.midi().defaultInputChannel === 0
            onActiveChanged: {
                if (active) {
                    if (controller.midi().defaultInputChannel !== 0) {
                        controller.midi().defaultInputChannel = 0
                    }
                } else {
                    inputChannelInput.value = 1
                    if (controller.midi().defaultInputChannel !== inputChannelInput.value) {
                        controller.midi().defaultInputChannel = inputChannelInput.value
                    }
                }
            }
        }
    }
    BlockRow {
        Text {
            text: "Input Channel:"
            width: parent.width - 40*dp
            verticalAlignment: Text.AlignVCenter
            enabled: !omniModeCheckbox.active
        }
        NumericInput {
            id: inputChannelInput
            height: parent.height
            width: 40*dp
            minimumValue: 1
            maximumValue: 16
            value: controller.midi().defaultInputChannel
            onValueChanged: {
                if (value !== controller.midi().defaultInputChannel) {
                    controller.midi().defaultInputChannel = value
                }
            }
            textReplacement: value === 0 ? "-" : ""
            enabled: !omniModeCheckbox.active
        }
    }
    BlockRow {
        Text {
            text: "Output Channel:"
            width: parent.width - 40*dp
            verticalAlignment: Text.AlignVCenter
        }
        NumericInput {
            height: parent.height
            width: 40*dp
            minimumValue: 1
            maximumValue: 16
            value: controller.midi().defaultOutputChannel
            onValueChanged: {
                if (value !== controller.midi().defaultOutputChannel) {
                    controller.midi().defaultOutputChannel = value
                }
            }
        }
    }
}  // MIDI column end
