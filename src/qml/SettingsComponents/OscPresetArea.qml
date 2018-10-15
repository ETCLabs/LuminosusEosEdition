import QtQuick 2.10
import CustomElements 1.0

import "../CustomControls"
import "../CustomBasics"

StretchColumn {
    property QtObject oscManager
    property bool showSpecificStatus: true

    signal openNewPresetArea(string originalPresetId)

    Item {
        implicitHeight: -1
        clip: true
        ListView {
            id: listView
            anchors.fill: parent
            model: oscManager.presetIds
            interactive: contentHeight > height
            delegate: Item {
                height: 40*dp
                width: listView.width
                property var preset: oscManager.getPreset(modelData)
                Rectangle {
                    anchors.fill: parent
                    anchors.bottomMargin: 10*dp
                    color: "#333"
                    CustomTouchArea {
                        anchors.fill: parent
                        onClick: oscManager.loadPreset(modelData)
                    }
                    StretchRow {
                        anchors.fill: parent
                        spacing: 7*dp
                        Rectangle {
                            width: 50*dp
                            color: "#555"
                            Text {
                                anchors.centerIn: parent
                                text: preset.type === "Custom" ? "OSC" : preset.type || "?"
                                font.pixelSize: 14*dp
                                fontSizeMode: Text.Fit
                            }
                        }
                        StretchText {
                            text: preset.name || preset.ip || "?"
                            elide: Text.ElideRight
                        }
                        IconButton {
                            width: 30*dp
                            iconName: "edit_icon"
                            onClick: openNewPresetArea(modelData)
                        }
                    }  // StretchRow

                    Rectangle {
                        anchors.fill: parent
                        color: "transparent"
                        border.width: 1
                        border.color: modelData === oscManager.currentPresetId ? (oscManager.connected ? "lightgreen" : "yellow") : "#444"
                    }
                }  // Rectangle
            }  // delegate Item
        }  // ListView
        Text {
            visible: listView.count <= 0
            anchors.centerIn: parent
            text: "No Connections\nAdd a new one!"
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 14*dp
            color: "#bbb"
        }
    }  // Item

    StretchColumn {
        visible: ["Custom", ""].indexOf(oscManager.currentPreset.type) !== -1 || !showSpecificStatus
        defaultSize: 30*dp
        height: implicitHeight
        StretchText {
            hAlign: Text.AlignHCenter
            text: oscManager.connected ? "Connected" : "Not Connected"
            color: oscManager.connected ? "lightgreen" : "red"
        }
        StretchText {
            text: oscManager.currentPreset.ip || "-"
            color: "#aaa"
        }
    }

    StretchColumn {
        visible: oscManager.currentPreset.type === "Eos" && showSpecificStatus
        defaultSize: 30*dp
        height: implicitHeight
        BlockRow {
            StretchText {
                text: "Connection:"
            }
            StretchText {
                hAlign: Text.AlignRight
                color: text === "Ok" ? "lightgreen" : "red"
                text: controller.eosManager().pingIsSuccessful
                      ? (controller.eosManager().latency < 200 ? "Ok" : "Slow")
                      : "Timeout"
            }
        }
        BlockRow {
            Text {
                text: oscManager.currentPreset.ip || "?"
                color: "#aaa"
            }
            StretchText {
                hAlign: Text.AlignHCenter
                text: "|"
            }
            Text {
                horizontalAlignment: Text.AlignRight
                text: controller.eosManager().consoleVersion || "unknown"
                color: "#aaa"
            }
        }
    }

    StretchColumn {
        visible: oscManager.currentPreset.type === "Hog4" && showSpecificStatus
        defaultSize: 30*dp
        height: implicitHeight
        BlockRow {
            StretchText {
                text: "Hog Console:"
            }
            StretchText {
                hAlign: Text.AlignRight
                color: controller.hogManager().connected ? "lightgreen" : "red"
                text: controller.hogManager().connected ? "Running" : "No Feedback"
            }
        }
        BlockRow {
            StretchText {
                text: oscManager.currentPreset.ip || "?"
                color: "#aaa"
            }
        }
    }

    StretchColumn {
        visible: ["X32", "X Air"].indexOf(oscManager.currentPreset.type) !== -1 && showSpecificStatus
        defaultSize: 30*dp
        height: implicitHeight
        BlockRow {
            StretchText {
                text: "Connection:"
            }
            StretchText {
                hAlign: Text.AlignHCenter
                color: text === "Ok" ? "lightgreen" : "red"
                text: controller.x32Manager().isConnected ? "Ok" : "Timeout"
            }
        }
        BlockRow {
            StretchText {
                text: "Name:"
            }
            StretchText {
                hAlign: Text.AlignHCenter
                text: controller.x32Manager().consoleInfoName
            }
        }
        BlockRow {
            StretchText {
                text: "Model:"
            }
            StretchText {
                hAlign: Text.AlignHCenter
                text: controller.x32Manager().consoleInfoModel
            }
        }
        BlockRow {
            StretchText {
                text: "Version:"
            }
            StretchText {
                hAlign: Text.AlignHCenter
                text: controller.x32Manager().consoleInfoVersion
            }
        }
    }

    StretchRow {
        height: 30*dp
        StretchText {
            text: "Connection Enabled:"
            color: "#aaa"
        }
        CheckBox {
            id: activeCheckbox
            width: 30*dp
            active: oscManager.enabled
            onActiveChanged: {
                if (active !== oscManager.enabled) {
                    oscManager.enabled = active
                }
            }
        }
    }

    StretchRow {
        height: 30*dp
        ButtonSideLine {
            text: "Add Connection"
            onClick: openNewPresetArea("")
        }
    }
}
