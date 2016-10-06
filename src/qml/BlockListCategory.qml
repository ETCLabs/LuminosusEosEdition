import QtQuick 2.5
import CustomElements 1.0

import "CustomControls"
import "CustomBasics"

StretchColumn {
	id: root
	height: 30*dp + (collapsed ? 0 : indentedContent.height)
	implicitHeight: 0  // do not stretch

	property bool collapsed: true
	property alias name: label.text
	property int level: 0

	property bool isCategory: true

	// add all children to indentedContent:
	default property alias content: indentedContent.children



	function addBlock(info, level, collapsed) {
		// check if block belongs to a subcategory:
		if (info.category.length > level) {
			var category = getCategory(info, level, collapsed)
			category.addBlock(info, level + 1, collapsed)
		} else {
			var entryComponent = Qt.createComponent("qrc:/qml/BlockListEntry.qml");
			var entryObject = entryComponent.createObject(indentedContent, {blockInfo: info})
		}
	}

	function getCategory(info, level, collapsed) {
		// try to find category in existing items:
		var items = indentedContent.children
		var name = info.category[level]
		for (var i = 0; i < items.length; ++i) {
			var item = items[i]
			if (item.isCategory && item.name === name) {
				// found it -> return it:
				return item
			}
		}
		// category not found -> create it:
		var categoryComponent = Qt.createComponent("qrc:/qml/BlockListCategory.qml");
		var categoryObject = categoryComponent.createObject(
					indentedContent, {name: name, level: level, collapsed: collapsed})
		return categoryObject
	}

    CustomTouchArea {
		id: row
		height: 30*dp
		StretchRow {  // row with left margin
			anchors.fill: parent
			leftMargin: 5*dp
			Item {  // arrow
				width: 30*dp
				Image {
					width: 10*dp
					height: 5*dp
					anchors.centerIn: parent
					source: "qrc:/images/down_arrow_slim.png"
					rotation: collapsed ? -90: 0
					Behavior on rotation {
						NumberAnimation {
							duration: 300
							easing.type: Easing.OutCubic
						}
					}
				}
			}

			Text {
				id: label
				width: parent.width - 30*dp
				verticalAlignment: Text.AlignVCenter
			}
		} // end row with left margin

		Rectangle {  // left line
			height: parent.height
			width: 1*dp
			color: "#555"
			visible: level > 0
		}

		onTouchUp: {
			if (!canceled) {
				collapsed = !collapsed
			}
		}
	}

	StretchColumn {  // indented content
		id: indentedContent
		height: implicitHeight
		leftMargin: 20*dp
		defaultSize: 30*dp
		visible: !root.collapsed

		// --- Children will be added here ---


	}  // end indented content
}
