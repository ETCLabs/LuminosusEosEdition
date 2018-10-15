import QtQuick 2.5
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0
import CustomStyle 1.0
import CustomElements 1.0
import "../CustomBasics"

CustomTouchArea {
	id: root
	implicitWidth: -1
	property var values: ["---"]
	property var texts: values
	property int currentIndex: 0
	property alias align: displayedText.horizontalAlignment
    property int scrollValue: 0

	property Item dropDownItem

	signal valueChanged(var value)

	onCurrentIndexChanged: {
		valueChanged(getValue())
	}

	function setValue(value) {
		var index = values.indexOf(value)
		if (index == -1) index = 0
        guiManager.setPropertyWithoutChangingBindings(root, "currentIndex", index)
	}

	function getValue() {
		return values[currentIndex]
	}


	Rectangle {  // border
		anchors.fill: parent
		color: "transparent"
		//border.width: 1*dp
		//border.color: "#555"
	}

	Image {  // arrows
		id: arrows
		height: 30*dp
		width: 10*dp
		source: "qrc:/images/combobox_arrows_blue.png"
		anchors.centerIn: parent
    }

    ColorOverlay {
        anchors.fill: arrows
        source: arrows
        color: Style.primaryActionColor
    }

	Text {  // text
		id: displayedText
		anchors.fill: parent
		anchors.rightMargin: 10*dp
		anchors.leftMargin: 10*dp
		font.pixelSize: 16*dp
		font.bold: true
		verticalAlignment: Text.AlignVCenter
		horizontalAlignment: Text.AlignHCenter
		color: root.enabled ? "lightgrey" : "#555"
        elide: Text.ElideMiddle

		text: root.texts[currentIndex]
		Component.onCompleted: {
			if (root.texts[currentIndex] === undefined) {
				console.log(root.texts, currentIndex)
			}
		}
	}

	// -------------- Touch Handling --------------------

    property int initialOffset: 0
	property bool indexChangedDuringTouch: false

	onTouchDown: {
		// check if DropDown is open:
		if (dropDownItem) {
			// it is open -> destroy it:
			dropDownItem.destroy()
			return
		}
		// don't change anything if not enabled:
		if (!enabled) return

		// save initial touch and offset position:
        initialOffset = currentIndex * 30*dp + touch.itemY
		indexChangedDuringTouch = false

		// create a new DropDown item and show it in right place:
		var window = Window.contentItem
		var xPos = mapToItem(window, 0, 0).x
		var yPos = mapToItem(window, 0, 0).y
		dropDownItem = dropDownComponent.createObject(window, {x: xPos, rootY: yPos})
        dropDownItem.forceActiveFocus()
	}

	onTouchMove: {
		// don't change anything if not enabled:
		if (!enabled) return

		// calculate offset from first value:
        var currentOffset = initialOffset - touch.itemY

		// calculate new Index:
		var newIndex = Math.floor((currentOffset + 15*dp) / (30*dp))
		// wrap around:
		var len = texts.length
		newIndex = ((newIndex%len)+len)%len
		// set if it changed:
		if (newIndex != currentIndex) {
            controller.playClickSound()
            guiManager.setPropertyWithoutChangingBindings(root, "currentIndex", newIndex)
			indexChangedDuringTouch = true
		}
	}

	onTouchUp: {
		// don't change anything if not enabled:
		if (!enabled) return

		// check if the touch was a tap:
        var dist = Math.abs(touch.itemOriginY - touch.itemY)
		var isTap = dist < 3*dp

		// if the index changed (even if it is the same as on the start now)
		// or if it was not a tap and there is a DropDown item:
		if (indexChangedDuringTouch || (!isTap && dropDownItem)) {
			// destroy the DropDown:
			dropDownItem.destroy()
		}
	}

	// ---------------------- DropDown Component ----------------

	Component {
		id: dropDownComponent
		Rectangle {  // root item and background
			id: dropDown
			property int rootY
			property int dropDownOffset: currentIndex * 30*dp
			Behavior on dropDownOffset {
				NumberAnimation {
					duration: 200
					easing.type: Easing.OutCubic
				}
			}

			y: rootY - dropDownOffset
			width: root.width
			height: optionListColumn.implicitHeight
			color: "#333"

			Column {  // holds the options
				id: optionListColumn
				width: parent.width
				Repeater {
					model: root.texts

					Text {
						width: parent.width
						height: 30*dp
						font.pixelSize: 16*dp
						font.bold: true
						verticalAlignment: Text.AlignVCenter
						horizontalAlignment: displayedText.horizontalAlignment
						color: "#fff"
                        elide: Text.ElideMiddle

						text: modelData
					}
				}
			}  // end option column

			Rectangle {  // lightblue border on current item
				y: dropDownOffset
				height: 30*dp
				width: parent.width
				color: "transparent"
				border.width: 1*dp
                border.color: Style.primaryHighlightColor
			}

            CustomTouchArea {
                width: 3000*dp
                height: 3000*dp
                anchors.centerIn: parent

                onTouchDown: {
                    dropDown.destroy()
                    touch.accepted = false
                }

                onScrollEvent: {
                    scrollValue += deltaY
                    if (scrollValue > 40) {
                        dropDown.previousItem()
                        scrollValue = 0
                    } else if (scrollValue < -40) {
                        dropDown.nextItem()
                        scrollValue = 0
                    }
                    scrollEventWasAccepted()
                }
            }

            CustomTouchArea {
				anchors.fill: parent

                onTouchDown: {
                    controller.playClickSound()
					// this is called when the DropDown was opened by a tap
					// and the user clicked on an option

					// calculate Index of clicked option:
                    var newIndex = Math.floor(touch.itemY / (30*dp))
					// set if it is not the current index:
                    if (newIndex != currentIndex) {
                        guiManager.setPropertyWithoutChangingBindings(root, "currentIndex", newIndex)
					}
					// destroy DropDown:
					dropDown.destroy()
				}
			}

            onActiveFocusChanged: {
                if (!activeFocus) {
                    dropDown.destroy()
                }
            }

            Keys.onEscapePressed: dropDown.destroy()

            function nextItem() {
                var len = texts.length
                var newIndex = (((currentIndex + 1)%len)+len)%len
                guiManager.setPropertyWithoutChangingBindings(root, "currentIndex", newIndex)
            }

            function previousItem() {
                var len = texts.length
                var newIndex = (((currentIndex - 1)%len)+len)%len
                guiManager.setPropertyWithoutChangingBindings(root, "currentIndex", newIndex)
            }

            Keys.onUpPressed: {
                previousItem()
            }
            Keys.onDownPressed: {
                nextItem()
            }
		}
	}
}
