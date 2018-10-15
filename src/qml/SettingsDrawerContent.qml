import QtQuick 2.4
import QtQuick.Window 2.2
import CustomElements 1.0
import "CustomControls"
import "CustomBasics"
import "SettingsComponents"


VerticalScrollView {
	contentItem: root
    anchors.topMargin: 10*dp

	StretchColumn {
		id: root
		width: parent.width
		height: implicitHeight
		defaultSize: 30*dp
		// height will be minimum height for all fixed sized items

		HorizontalDivider {  // ------------------------------------------------------
			visible: false
        }

        SettingsCategory {
            name: "Lighting Console"

            EosSettings {
                visible: !parent.collapsed
            }
        }

        HorizontalDivider {  // ------------------------------------------------------
        }

        SettingsCategory {
            name: "Audio Console"

            X32Settings {
                visible: !parent.collapsed
            }
        }

        HorizontalDivider {  // ------------------------------------------------------
        }

        SettingsCategory {
            name: "Custom OSC"

            OscSettings {
                visible: !parent.collapsed
            }
        }

        HorizontalDivider {  // ------------------------------------------------------
            visible: Qt.platform.os !== "ios"
        }

        SettingsCategory {
            name: "MIDI Mapping"
            visible: Qt.platform.os !== "ios"

            MidiMappingSettings {
                visible: !parent.collapsed
            }
        }

		HorizontalDivider {  // ------------------------------------------------------
            visible: Qt.platform.os !== "ios"
		}

        SettingsCategory {
            name: "MIDI Devices"
            visible: Qt.platform.os !== "ios"

            MidiSettings {
                visible: !parent.collapsed
            }
        }

		HorizontalDivider {  // ------------------------------------------------------
        }

        SettingsCategory {
            name: "GUI"

            GuiSettings {
                visible: !parent.collapsed
            }
        }

		HorizontalDivider {  // ------------------------------------------------------
        }

        SettingsCategory {
            name: "About"
            collapsed: true

            AboutSection {
                visible: !parent.collapsed
            }
        }

        Item {
            height: 80*dp
            Rectangle {
                anchors.fill: parent
                anchors.margins: 15*dp
                color: "#333"
                border.width: 1
                border.color: forumTouchArea.mouseOver ? "lightgreen" : "#777"

                Text {
                    anchors.fill: parent
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    text: "Questions?\n→ ETC Labs Forum"
                }

                CustomTouchArea {
                    id: forumTouchArea
                    anchors.fill: parent
                    mouseOverEnabled: true
                    onClick: Qt.openUrlExternally("https://community.etcconnect.com/etclabs/f/etc-labs-software")
                }
            }
        }
        Item {
            height: 20*dp
        }

        BlockRow {
            leftMargin: 20*dp
            rightMargin: 20*dp
            Rectangle {
                implicitWidth: -1  // stretch
                color: Qt.rgba(1, 0, 0, 0.2)
                ButtonBottomLine {
                    anchors.fill: parent
                    text: "QUIT"
                    onPress: Qt.quit()
                }
            }
        }

	}  // end main Column
}  // end Flickable

