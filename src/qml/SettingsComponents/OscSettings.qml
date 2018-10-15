import QtQuick 2.5
import CustomElements 1.0

import "../CustomControls"
import "../CustomBasics"


StretchColumn {
    leftMargin: 15*dp
    rightMargin: 15*dp
    defaultSize: 30*dp
    height: implicitHeight

    OscConnectionArea {
        oscManager: controller.customOsc()
        showSpecificStatus: false
    }

//    BlockRow {
//        visible: !controller.sendCustomOscToEos
//        Text {
//            text: "IP Address"
//            width: parent.width * 0.5
//            verticalAlignment: Text.AlignVCenter
//        }
//        TextInput {
//            width: parent.width * 0.5
//            inputMethodHints: Qt.ImhFormattedNumbersOnly
//            text: controller.customOsc().getIpAddressString()
//            onDisplayTextChanged: controller.customOsc().setIpAddress(displayText)
//            font.pixelSize: 16*dp
//        }
//    }
//    BlockRow {
//        visible: !controller.sendCustomOscToEos
//        Text {
//            text: "Protocol"
//            width: parent.width * 0.5
//            verticalAlignment: Text.AlignVCenter
//        }
//        ComboBox2 {
//            id: protocolComboBox
//            width: parent.width * 0.5
//            height: 30*dp
//            values: controller.customOsc().getProtocolNames()
//            property bool initialized: false
//            Component.onCompleted: {
//                setValue(controller.customOsc().getProtocolString())
//                initialized = true
//            }
//            onValueChanged: {
//                if (!initialized) return
//                if (value !== controller.customOsc().getProtocolString()) {
//                    controller.customOsc().setProtocolByString(value);
//                }
//            }
//        }
//    }
//    BlockRow {
//        visible: (protocolComboBox.currentIndex === 0) && !controller.sendCustomOscToEos
//        Text {
//            text: "Tx Port"
//            width: parent.width / 2
//            verticalAlignment: Text.AlignVCenter
//        }
//        NumericInput {
//            width: parent.width / 2
//            minimumValue: 0
//            maximumValue: 65535
//            value: controller.customOsc().getUdpTxPort()
//            onValueChanged: {
//                if (value !== controller.customOsc().getUdpTxPort()) {
//                    controller.customOsc().setUdpTxPort(value)
//                }
//            }
//        }
//    }
//    BlockRow {
//        visible: (protocolComboBox.currentIndex === 0) && !controller.sendCustomOscToEos
//        Text {
//            text: "Rx Port"
//            width: parent.width / 2
//            verticalAlignment: Text.AlignVCenter
//        }
//        NumericInput {
//            width: parent.width / 2
//            minimumValue: 0
//            maximumValue: 65535
//            value: controller.customOsc().getUdpRxPort()
//            onValueChanged: {
//                if (value !== controller.customOsc().getUdpRxPort()) {
//                    controller.customOsc().setUdpRxPort(value)
//                }
//            }
//        }
//    }
//    BlockRow {
//        visible: (protocolComboBox.currentIndex !== 0) && !controller.sendCustomOscToEos
//        Text {
//            text: "Port"
//            width: parent.width / 2
//            verticalAlignment: Text.AlignVCenter
//        }
//        NumericInput {
//            width: parent.width / 2
//            minimumValue: 0
//            maximumValue: 65535
//            value: controller.customOsc().getTcpPort()
//            onValueChanged: {
//                if (value !== controller.customOsc().getTcpPort()) {
//                    controller.customOsc().setTcpPort(value)
//                }
//            }
//        }
//    }
}  // end OSC Settings Column
