import QtQuick 2.4
import QtQuick.Window 2.2
import CustomElements 1.0
import "CustomControls"
import "CustomBasics"


VerticalScrollView {
	contentItem: root

	StretchColumn {
		id: root
		width: parent.width
		height: implicitHeight
		defaultSize: 30*dp
		// height will be minimum height for all fixed sized items

		Text {
			height: 30*dp
			text: "ArtNet"
			color: "#fff"
			horizontalAlignment: Text.AlignHCenter
			verticalAlignment: Text.AlignVCenter
			font.family: "Quicksand"
			font.pixelSize: 22*dp
			visible: false
		}
		StretchColumn {
			leftMargin: 15*dp
			rightMargin: 15*dp
			defaultSize: 30*dp
			height: implicitHeight
			visible: false

			BlockRow {
				Text {
					text: "Subnet"
					width: parent.width * 0.7
					verticalAlignment: Text.AlignVCenter
				}
				TextInput {
					text: "0"
					width: parent.width * 0.3
					inputMethodHints: Qt.ImhDigitsOnly
					onTextChanged: {
						// TODO
					}
				}
			}
			BlockRow {
				Text {
					text: "Net"
					width: parent.width * 0.7
					verticalAlignment: Text.AlignVCenter
				}
				TextInput {
					text: "0"
					width: parent.width * 0.3
					inputMethodHints: Qt.ImhDigitsOnly
					onTextChanged: {
						// TODO
					}
				}
			}
			BlockRow {
				Text {
					text: "Broadcast"
					width: parent.width / 2
					verticalAlignment: Text.AlignVCenter
				}
				Switch {
					height: parent.height
					width: parent.width / 2
				}
			}
		}  // end ArtNet Settings column

		HorizontalDivider {  // ------------------------------------------------------
			visible: false
		}

		Text {
			height: 30*dp
			text: "OSC"
			color: "#fff"
			horizontalAlignment: Text.AlignHCenter
			verticalAlignment: Text.AlignTop
			font.family: "Quicksand"
			font.pixelSize: 22*dp
		}
		StretchColumn {
			leftMargin: 15*dp
			rightMargin: 15*dp
			defaultSize: 30*dp
			height: implicitHeight

			BlockRow {
				Text {
					text: "IP Address"
					width: parent.width * 0.5
					verticalAlignment: Text.AlignVCenter
				}
				TextInput {
					width: parent.width * 0.5
					inputMethodHints: Qt.ImhFormattedNumbersOnly
					text: controller.osc().getIpAddressString()
					onTextChanged: controller.osc().setIpAddress(text)
					font.pixelSize: 16*dp
				}
			}
			BlockRow {
				Text {
					text: "Protocol"
					width: parent.width * 0.5
					verticalAlignment: Text.AlignVCenter
				}
				ComboBox2 {
					id: protocolComboBox
					width: parent.width * 0.5
					height: 30*dp
					values: controller.osc().getProtocolNames()
					property bool initialized: false
					Component.onCompleted: {
						setValue(controller.osc().getProtocolString())
						initialized = true
					}
					onValueChanged: {
						if (!initialized) return
						if (value !== controller.osc().getProtocolString()) {
							controller.osc().setProtocolByString(value);
						}
					}
				}
			}
			BlockRow {
				visible: protocolComboBox.currentIndex === 0
				Text {
					text: "Tx Port"
					width: parent.width / 2
					verticalAlignment: Text.AlignVCenter
				}
				NumericInput {
					width: parent.width / 2
					minimumValue: 0
					maximumValue: 65535
					value: controller.osc().getUdpTxPort()
					onValueChanged: {
						if (value !== controller.osc().getUdpTxPort()) {
							controller.osc().setUdpTxPort(value)
						}
					}
				}
			}
			BlockRow {
				visible: protocolComboBox.currentIndex === 0
				Text {
					text: "Rx Port"
					width: parent.width / 2
					verticalAlignment: Text.AlignVCenter
				}
				NumericInput {
					width: parent.width / 2
					minimumValue: 0
					maximumValue: 65535
					value: controller.osc().getUdpRxPort()
					onValueChanged: {
						if (value !== controller.osc().getUdpRxPort()) {
							controller.osc().setUdpRxPort(value)
						}
					}
				}
			}
			BlockRow {
				visible: protocolComboBox.currentIndex !== 0
				Text {
					text: "Port"
					width: parent.width / 2
					verticalAlignment: Text.AlignVCenter
				}
				NumericInput {
					width: parent.width / 2
					minimumValue: 0
					maximumValue: 65535
					value: controller.osc().getTcpPort()
					onValueChanged: {
						if (value !== controller.osc().getTcpPort()) {
							controller.osc().setTcpPort(value)
						}
					}
				}
			}
		}  // end OSC Settings Column

		HorizontalDivider {  // ------------------------------------------------------
		}

		Text {
			height: 30*dp
			text: "MIDI"
			color: "#fff"
			horizontalAlignment: Text.AlignHCenter
			verticalAlignment: Text.AlignTop
			font.family: "Quicksand"
			font.pixelSize: 22*dp
		}

		StretchColumn {
			leftMargin: 15*dp
			rightMargin: 15*dp
			defaultSize: 30*dp
			height: implicitHeight

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

		HorizontalDivider {  // ------------------------------------------------------
		}

		Text {
			height: 40*dp
			text: "Debugging"
			color: "#fff"
			horizontalAlignment: Text.AlignHCenter
			verticalAlignment: Text.AlignVCenter
			font.family: "Quicksand"
			font.pixelSize: 22*dp
		}

		StretchColumn {
			leftMargin: 15*dp
			rightMargin: 15*dp
			defaultSize: 30*dp
			height: implicitHeight
			// height will be sum of all heights

			BlockRow {
				Text {
					text: "Fullscreen"
					width: parent.width / 2
					verticalAlignment: Text.AlignVCenter
				}
				Switch {
					onActiveChanged: {
						active ? mainWindow.showFullScreen() : mainWindow.showNormal()
					}
				}
			}
			BlockRow {
				Text {
					text: "Engine"
					width: parent.width / 2
					verticalAlignment: Text.AlignVCenter
				}
				Switch {
					height: parent.height
					width: parent.width / 2
					active: true
					onActiveChanged: {
						active ? controller.engine().start() : controller.engine().stop()
					}
				}
			}
			BlockRow {
				Text {
					text: "Scaling"
					width: parent.width / 2
					verticalAlignment: Text.AlignVCenter
				}
				NumericInput {
					height: parent.height
					width: parent.width / 2
					value: dp
					minimumValue: 0.5
					maximumValue: 3
					decimals: 1
					onValueChanged: {
						if (value !== dp) {
							controller.setGuiScaling(value)
						}
					}
				}
			}
			BlockRow {
				Text {
					text: "Graphics"
					width: parent.width / 2
					verticalAlignment: Text.AlignVCenter
				}
				ComboBox2 {
					width: parent.width / 2
					values: [1, 2, 3]
					texts: ["Low", "Mid", "High"]
					Component.onCompleted: setValue(GRAPHICAL_EFFECTS_LEVEL)
					onValueChanged: {
						if (value !== GRAPHICAL_EFFECTS_LEVEL) {
							controller.setQmlContextProperty("GRAPHICAL_EFFECTS_LEVEL", value)
						}
					}
				}
			}
			BlockRow {
				ButtonBottomLine {
					width: parent.width
					text: "Conn. External Input"
					onPress: {
						controller.waitForExternalEvent()
						toast.displayToast("1. Touch GUI control\n2. Use External Input")
					}
				}
			}
			BlockRow {
				ButtonBottomLine {
					width: parent.width
					text: "Release External Input"
					onPress: {
						controller.waitToReleaseNextControl()
						toast.displayToast("Touch GUI control\nto release all connections.")
					}
				}
			}
		}  // end App Settings column

		HorizontalDivider {  // ------------------------------------------------------
		}

		Text {
			height: 30*dp
			text: "About"
			color: "#fff"
			horizontalAlignment: Text.AlignHCenter
			verticalAlignment: Text.AlignTop
			font.family: "Quicksand"
			font.pixelSize: 22*dp
		}

		StretchColumn {
			leftMargin: 15*dp
			rightMargin: 15*dp
			defaultSize: 30*dp
			height: implicitHeight

			BlockRow {
				Text {
					text: "Version:"
					width: parent.width / 2
					verticalAlignment: Text.AlignVCenter
				}
				Text {
					text: controller.getVersionString()
					width: parent.width / 2
					verticalAlignment: Text.AlignVCenter
				}
			}

			BlockRow {
				Text {
					text: "Author:"
					width: parent.width / 2
					verticalAlignment: Text.AlignVCenter
				}
				Text {
					text: "Tim Henning"
					width: parent.width / 2
					verticalAlignment: Text.AlignVCenter
				}
			}

			BlockRow {
				Text {
					text: "mail@luminosus.org"
					width: parent.width
					verticalAlignment: Text.AlignVCenter
				}
			}

			BlockRow {
				Text {
					text: "© ETC GmbH"
					width: parent.width
					verticalAlignment: Text.AlignVCenter
				}
			}

			BlockRow {
				Text {
					text: "www.etcconnect.com"
					width: parent.width
					verticalAlignment: Text.AlignVCenter
					font.underline: true
					MouseArea {
						anchors.fill: parent
						onClicked: Qt.openUrlExternally("http://www.etcconnect.com")
					}
				}
			}
		}  // end About column
	}  // end main Column
}  // end Flickable

