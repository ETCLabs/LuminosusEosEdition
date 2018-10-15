import QtQuick 2.5
import CustomElements 1.0

import "../CustomControls"
import "../CustomBasics"


StretchColumn {
    leftMargin: 15*dp
    rightMargin: 15*dp
    defaultSize: 30*dp
    height: implicitHeight

    BlockRow {
        StretchText {
            text: "Send sACN:"
        }
        CheckBox {
            width: 30*dp
            active: controller.output().sAcnEnabled
            onActiveChanged: {
                if (active !== controller.output().sAcnEnabled) {
                    controller.output().sAcnEnabled = active
                }
            }
        }
    }
    BlockRow {
        visible: controller.output().sAcnEnabled
        StretchText {
            text: "sACN Start Universe:"
        }
        NumericInput {
            implicitWidth: 0
            width: 55*dp
            value: controller.output().sAcnStartUniverse
            minimumValue: 1
            maximumValue: 63999 - 16
            onValueChanged: {
                if (value !== controller.output().sAcnStartUniverse) {
                    controller.output().sAcnStartUniverse = value
                }
            }
        }
    }
    BlockRow {
        visible: controller.output().sAcnEnabled
        StretchText {
            text: "sACN Priority:"
        }
        NumericInput {
            implicitWidth: 0
            width: 40*dp
            value: controller.output().sAcnPriority
            minimumValue: 1
            maximumValue: 200
            onValueChanged: {
                if (value !== controller.output().sAcnPriority) {
                    controller.output().sAcnPriority = value
                }
            }
        }
    }
    BlockRow {
        StretchText {
            text: "Send ArtNet:"
        }
        CheckBox {
            width: 30*dp
            active: controller.output().artnetEnabled
            onActiveChanged: {
                if (active !== controller.output().artnetEnabled) {
                    controller.output().artnetEnabled = active
                }
            }
        }
    }
    BlockRow {
        visible: controller.output().artnetEnabled
        StretchText {
            text: "ArtNet Net:"
        }
        NumericInput {
            implicitWidth: 0
            width: 40*dp
            value: controller.output().artnetNet
            minimumValue: 0
            maximumValue: 127
            onValueChanged: {
                if (value !== controller.output().artnetNet) {
                    controller.output().artnetNet = value
                }
            }
        }
    }
    BlockRow {
        visible: controller.output().artnetEnabled
        StretchText {
            text: "ArtNet Subnet:"
        }
        NumericInput {
            implicitWidth: 0
            width: 40*dp
            value: controller.output().artnetSubnet
            minimumValue: 0
            maximumValue: 15
            onValueChanged: {
                if (value !== controller.output().artnetSubnet) {
                    controller.output().artnetSubnet = value
                }
            }
        }
    }
    BlockRow {
        visible: controller.output().artnetEnabled
        StretchText {
            text: "Broadcast ArtNet:"
        }
        CheckBox {
            width: 30*dp
            active: controller.output().broadcastArtnet
            onActiveChanged: {
                if (active !== controller.output().broadcastArtnet) {
                    controller.output().broadcastArtnet = active
                }
            }
        }
    }
    BlockRow {
        StretchText {
            text: "Discovered Nodes:"
            color: "#aaa"
        }
        Text {
            width: 30*dp
            text: controller.output().discoveredNodes.length
            horizontalAlignment: Text.AlignRight
            color: "#aaa"
        }
    }
    Repeater {
        visible: controller.output().artnetEnabled
        model: controller.output().discoveredNodes

        Item {
            height: 70*dp
            Rectangle {
                anchors.fill: parent
                anchors.topMargin: 3*dp
                color: Qt.rgba(1, 1, 1, 0.1)
                border.width: 1
                border.color: "#888"
                StretchColumn {
                    anchors.fill: parent
                    anchors.leftMargin: 4*dp
                    Text {
                        height: parent.height * 0.4
                        text: modelData["longName"]
                        verticalAlignment: Text.AlignVCenter
                        fontSizeMode: Text.Fit
                        elide: Text.ElideRight
                    }
                    Text {
                        height: parent.height * 0.3
                        text: "IP: " + modelData["ipString"] + " Ports: " + modelData["numPorts"]
                        font.pixelSize: 16*dp
                        color: "#ddd"
                        verticalAlignment: Text.AlignVCenter
                    }
                    Text {
                        height: parent.height * 0.3
                        text: "Net: " + modelData["net"] + " Subnet: " + modelData["subnet"] + " Uni: " + modelData["universe1"]
                        font.pixelSize: 16*dp
                        color: "#ddd"
                        verticalAlignment: Text.AlignVCenter
                    }
                }
            }
        }
    }
}  // end ArtNet Settings column
