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
        oscManager: controller.audioConsole()
    }

//    BlockRow {
//        Text {
//            text: "IP Address:"
//            width: parent.width * 0.5
//            verticalAlignment: Text.AlignVCenter
//        }
//        TextInput {
//            width: parent.width * 0.5
//            inputMethodHints: Qt.ImhFormattedNumbersOnly
//            text: controller.audioConsole().getIpAddressString()
//            onDisplayTextChanged: controller.audioConsole().setIpAddress(displayText)
//            font.pixelSize: 16*dp
//        }
//    }
//    BlockRow {
//        Text {
//            text: "Console Type:"
//            width: parent.width * 0.5
//            verticalAlignment: Text.AlignVCenter
//        }
//        ComboBox2 {
//            width: parent.width * 0.5
//            height: 30*dp
//            values: [10023, 10024]
//            texts: ["X32", "X Air"]
//            property bool initialized: false
//            Component.onCompleted: {
//                setValue(controller.audioConsole().getUdpTxPort())
//                initialized = true
//            }
//            onValueChanged: {
//                if (!initialized) return
//                if (value !== controller.audioConsole().getUdpTxPort()) {
//                    controller.audioConsole().setUdpTxPort(value);
//                    controller.audioConsole().setUdpRxPort(value);
//                }
//            }
//        }
//    }
//    Item {  // -----------------------------------------------
//        height: 20*dp
//    }
//    BlockRow {
//        StretchText {
//            text: "Connection:"
//        }
//        StretchText {
//            hAlign: Text.AlignHCenter
//            color: text === "Ok" ? "lightgreen" : "red"
//            text: controller.x32Manager().isConnected ? "Ok" : "Timeout"
//        }
//    }
//    BlockRow {
//        StretchText {
//            text: "Name:"
//        }
//        StretchText {
//            hAlign: Text.AlignHCenter
//            text: controller.x32Manager().consoleInfoName
//        }
//    }
//    BlockRow {
//        StretchText {
//            text: "Model:"
//        }
//        StretchText {
//            hAlign: Text.AlignHCenter
//            text: controller.x32Manager().consoleInfoModel
//        }
//    }
//    BlockRow {
//        StretchText {
//            text: "Version:"
//        }
//        StretchText {
//            hAlign: Text.AlignHCenter
//            text: controller.x32Manager().consoleInfoVersion
//        }
//    }
}  // end X32 Connection Column
