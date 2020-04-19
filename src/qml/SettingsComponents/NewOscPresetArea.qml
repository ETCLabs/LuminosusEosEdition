import QtQuick 2.5
import CustomElements 1.0

import "../CustomControls"
import "../CustomBasics"

StretchColumn {
    property QtObject oscManager
    property string originalPresetId: ""
    property var preset: originalPresetId ? oscManager.getPreset(originalPresetId) : ({})

    signal closed

    StretchRow {
        height: 45*dp
        StretchText {
            text: originalPresetId ? "Edit Connection" : "New Connection:"
            hAlign: Text.AlignHCenter
            vAlign: Text.AlignVCenter
            font.bold: true
            font.underline: true
            font.pixelSize: 22*dp
        }
    }

    StretchRow {
        height: 30*dp
        StretchText {
            text: "Name:"
        }
        TextInput {
            id: presetNameInput
            width: 140*dp
            text: preset.name || ""
        }
    }

    StretchRow {
        height: 30*dp
        StretchText {
            text: "Type:"
        }
        ComboBox2 {
            id: typeCombobox
            width: 90*dp
            implicitWidth: 0
            values: oscManager.getAvailableTypes()
            currentIndex: Math.max(values.indexOf(preset.type), 0)
        }
    }
    StretchRow {
        height: 30*dp
        Text {
            text: "IP Address:"
            width: parent.width * 0.5
            verticalAlignment: Text.AlignVCenter
        }
        TextInput {
            id: ipInput
            width: parent.width * 0.5
            inputMethodHints: Qt.ImhFormattedNumbersOnly
            text: preset.ip || "127.0.0.1"
            font.pixelSize: 16*dp
        }
    }

    // -----------------------------------------------------------------

    StretchColumn {
        visible: typeCombobox.values[typeCombobox.currentIndex] === "Custom"
        implicitHeight: -1
        StretchRow {
            height: 30*dp
            Text {
                text: "Protocol"
                width: parent.width * 0.5
                verticalAlignment: Text.AlignVCenter
            }
            ComboBox2 {
                id: protocolComboBox
                width: parent.width * 0.5
                height: 30*dp
                values: controller.customOsc().getProtocolNames()
                currentIndex: Math.max(values.indexOf(preset.protocol), 0)
            }
        }
        StretchRow {
            height: 30*dp
            visible: protocolComboBox.currentIndex === 0
            Text {
                text: "Tx Port"
                width: parent.width / 2
                verticalAlignment: Text.AlignVCenter
            }
            NumericInput {
                id: customUdpTxInput
                width: parent.width / 2
                minimumValue: 0
                maximumValue: 65535
                value: preset.udpTxPort || 8001
            }
        }
        StretchRow {
            height: 30*dp
            visible: protocolComboBox.currentIndex === 0
            Text {
                text: "Rx Port"
                width: parent.width / 2
                verticalAlignment: Text.AlignVCenter
            }
            NumericInput {
                id: customUdpRxInput
                width: parent.width / 2
                minimumValue: 0
                maximumValue: 65535
                value: preset.udpRxPort || 8000
            }
        }
        StretchRow {
            height: 30*dp
            visible: protocolComboBox.currentIndex !== 0
            Text {
                text: "Port"
                width: parent.width / 2
                verticalAlignment: Text.AlignVCenter
            }
            NumericInput {
                id: customTcpInput
                width: parent.width / 2
                minimumValue: 0
                maximumValue: 65535
                value: preset.tcpPort || 3032
            }
        }
        Item {
            implicitHeight: -1
        }

        StretchRow {
            height: 30*dp
            IconButton {
                visible: originalPresetId
                width: 30*dp
                iconName: "trash_icon"
                onClick: {
                    oscManager.removePreset(originalPresetId)
                    closed()
                }
            }
            ButtonBottomLine {
                text: "Cancel"
                onClick: closed()
                color: "#777"
            }
            ButtonBottomLine {
                text: originalPresetId ? "Save" : "Add"
                onClick: {
                    Qt.inputMethod.commit()
                    var name = presetNameInput.text
                    var ip = ipInput.text
                    var protocol = protocolComboBox.getValue()
                    var udpTxPort = customUdpTxInput.value
                    var udpRxPort = customUdpRxInput.value
                    var tcpPort = customTcpInput.value
                    oscManager.createAndLoadPreset("Custom", name, protocol, ip, udpTxPort, udpRxPort, tcpPort, originalPresetId)
                    closed()
                }
            }
        }
    }

    // -----------------------------------------------------------------

    StretchColumn {
        visible: typeCombobox.values[typeCombobox.currentIndex] === "Eos"
        implicitHeight: -1
        StretchRow {
            height: 30*dp
            StretchText {
                text: "OSC Version:"
            }
            ComboBox2 {
                id: eosProtocolComboBox
                width: 90*dp
                implicitWidth: 0
                height: 30*dp
                values: controller.lightingConsole().getProtocolNames()
                currentIndex: Math.max(values.indexOf(preset.protocol), 0)
            }
        }
        StretchRow {
            height: 30*dp
            visible: eosProtocolComboBox.currentIndex === 0
            Text {
                text: "Tx Port"
                width: parent.width / 2
                verticalAlignment: Text.AlignVCenter
            }
            NumericInput {
                id: eosUdpTxInput
                width: parent.width / 2
                minimumValue: 0
                maximumValue: 65535
                value: preset.udpTxPort || 8001
            }
        }
        StretchRow {
            height: 30*dp
            visible: eosProtocolComboBox.currentIndex === 0
            Text {
                text: "Rx Port"
                width: parent.width / 2
                verticalAlignment: Text.AlignVCenter
            }
            NumericInput {
                id: eosUdpRxInput
                width: parent.width / 2
                minimumValue: 0
                maximumValue: 65535
                value: preset.udpRxPort || 8000
            }
        }
        StretchRow {
            implicitHeight: -1
            Text {
                text: "Click here to see the manual\nfor help with the connection."
                width: parent.width
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                font.underline: true
                font.pixelSize: 14*dp
                CustomTouchArea {
                    anchors.fill: parent
                    onClick: Qt.openUrlExternally("https://www.luminosus.org/downloads/Manual_en.pdf")
                }
            }
        }
        StretchRow {
            height: 30*dp
            IconButton {
                visible: originalPresetId
                width: 30*dp
                iconName: "trash_icon"
                onClick: {
                    oscManager.removePreset(originalPresetId)
                    closed()
                }
            }
            ButtonBottomLine {
                text: "Cancel"
                onClick: closed()
                color: "#777"
            }
            ButtonBottomLine {
                text: originalPresetId ? "Save" : "Add"
                onClick: {
                    Qt.inputMethod.commit()
                    var name = presetNameInput.text
                    var ip = ipInput.text
                    var udpTxPort = eosUdpTxInput.value
                    var udpRxPort = eosUdpRxInput.value
                    var protocol = eosProtocolComboBox.getValue()
                    var tcpPort = 3032
                    oscManager.createAndLoadPreset("Eos", name, protocol, ip, udpTxPort, udpRxPort, tcpPort, originalPresetId)
                    closed()
                }
            }
        }
    }

    // -----------------------------------------------------------------

    StretchColumn {
        visible: typeCombobox.values[typeCombobox.currentIndex] === "Hog4"
        implicitHeight: -1
        StretchRow {
            height: 30*dp
            Text {
                text: "Protocol"
                width: parent.width * 0.5
                verticalAlignment: Text.AlignVCenter
            }
            ComboBox2 {
                id: hogProtocolComboBox
                width: parent.width * 0.5
                height: 30*dp
                values: controller.customOsc().getProtocolNames()
                texts: ["UDP", "TCP/HDR", "TCP/SLIP"]
                currentIndex: Math.max(values.indexOf(preset.protocol), 0)
            }
        }
        StretchRow {
            height: 30*dp
            Text {
                text: hogProtocolComboBox.currentIndex === 0 ? "Send Port" : "In & Out Port"
                width: parent.width / 2
                verticalAlignment: Text.AlignVCenter
            }
            NumericInput {
                id: hogTxInput
                width: parent.width / 2
                minimumValue: 0
                maximumValue: 65535
                value: (hogProtocolComboBox.currentIndex === 0 ? preset.udpTxPort : preset.tcpPort) || 7001
            }
        }
        StretchRow {
            height: 30*dp
            visible: hogProtocolComboBox.currentIndex === 0
            Text {
                text: "Receive Port"
                width: parent.width / 2
                verticalAlignment: Text.AlignVCenter
            }
            NumericInput {
                id: hogRxInput
                width: parent.width / 2
                minimumValue: 0
                maximumValue: 65535
                value: preset.udpRxPort || 7001
            }
        }
        Item {
            implicitHeight: -1
        }
        StretchRow {
            height: 30*dp
            IconButton {
                visible: originalPresetId
                width: 30*dp
                iconName: "trash_icon"
                onClick: {
                    oscManager.removePreset(originalPresetId)
                    closed()
                }
            }
            ButtonBottomLine {
                text: "Cancel"
                onClick: closed()
                color: "#777"
            }
            ButtonBottomLine {
                text: originalPresetId ? "Save" : "Add"
                onClick: {
                    Qt.inputMethod.commit()
                    var name = presetNameInput.text
                    var ip = ipInput.text
                    var protocol = hogProtocolComboBox.getValue()
                    var udpTxPort = hogTxInput.value
                    var udpRxPort = hogRxInput.value
                    var tcpPort = hogTxInput.value
                    oscManager.createAndLoadPreset("Hog4", name, protocol, ip, udpTxPort, udpRxPort, tcpPort, originalPresetId)
                    closed()
                }
            }
        }
    }

    // -----------------------------------------------------------------

    StretchColumn {
        visible: typeCombobox.values[typeCombobox.currentIndex] === "X32"
        implicitHeight: -1
        Item {
            implicitHeight: -1
        }
        StretchRow {
            height: 30*dp
            IconButton {
                visible: originalPresetId
                width: 30*dp
                iconName: "trash_icon"
                onClick: {
                    oscManager.removePreset(originalPresetId)
                    closed()
                }
            }
            ButtonBottomLine {
                text: "Cancel"
                onClick: closed()
                color: "#777"
            }
            ButtonBottomLine {
                text: originalPresetId ? "Save" : "Add"
                onClick: {
                    Qt.inputMethod.commit()
                    var name = presetNameInput.text
                    var ip = ipInput.text
                    var protocol = "UDP"
                    var udpTxPort = 10023
                    var udpRxPort = 10023
                    var tcpPort = 3032
                    oscManager.createAndLoadPreset("X32", name, protocol, ip, udpTxPort, udpRxPort, tcpPort, originalPresetId)
                    closed()
                }
            }
        }
    }

    // -----------------------------------------------------------------

    StretchColumn {
        visible: typeCombobox.values[typeCombobox.currentIndex] === "X Air"
        implicitHeight: -1
        Item {
            implicitHeight: -1
        }
        StretchRow {
            height: 30*dp
            IconButton {
                visible: originalPresetId
                width: 30*dp
                iconName: "trash_icon"
                onClick: {
                    oscManager.removePreset(originalPresetId)
                    closed()
                }
            }
            ButtonBottomLine {
                text: "Cancel"
                onClick: closed()
                color: "#777"
            }
            ButtonBottomLine {
                text: originalPresetId ? "Save" : "Add"
                onClick: {
                    Qt.inputMethod.commit()
                    var name = presetNameInput.text
                    var ip = ipInput.text
                    var protocol = "UDP"
                    var udpTxPort = 10024
                    var udpRxPort = 10024
                    var tcpPort = 3032
                    oscManager.createAndLoadPreset("X Air", name, protocol, ip, udpTxPort, udpRxPort, tcpPort, originalPresetId)
                    closed()
                }
            }
        }
    }

    // -----------------------------------------------------------------
}
