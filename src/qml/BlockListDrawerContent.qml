import QtQuick 2.5
import CustomElements 1.0

import "CustomControls"
import "CustomBasics"


StretchColumn {
	anchors.fill: parent

    property bool showCombinations: false

    function saveCombination() {
        Qt.inputMethod.commit()
        controller.projectManager().saveCombination(combinationNameInput.text)
        combinationNameInput.text = ""
    }

    BlockRow {
        height: 40*dp
        implicitHeight: 0  // do not stretch

        CustomTouchArea {
            implicitWidth: -1
            onClick: showCombinations = false
            Rectangle {
                anchors.fill: parent
                color: !showCombinations ? Qt.hsva(0, 0, 1, 0.1) : "transparent"
                Text {
                    anchors.centerIn: parent
                    text: "Blocks"
                }
            }
        }
        Rectangle {
            width: 1
            color: "#aaa"
        }
        CustomTouchArea {
            implicitWidth: -1
            onClick: showCombinations = true
            Rectangle {
                anchors.fill: parent
                color: showCombinations ? Qt.hsva(0, 0, 1, 0.1) : "transparent"
                Text {
                    anchors.centerIn: parent
                    text: "Combinations"
                }
            }
        }
    }

    Rectangle {
        height: 1
        width: parent.width
        color: "#aaa"
    }
    Item {
        height: 10*dp
    }

	// ---------------- Search ------------

	BlockRow {
		height: 30*dp
		implicitHeight: 0  // do not stretch
		leftMargin: 10*dp
        visible: !showCombinations
		Item {
			implicitWidth: -1
			TextInput {
				id: searchField
				anchors.fill: parent
				hintText: "Search"

				onDisplayTextChanged: {
					if (text.length === 0) {
						searchTimer.stop()
						blockListContainer.showAllBlocks()
					} else {
						searchTimer.restart()
					}
				}

				Timer {
					id: searchTimer
					repeat: false
					interval: 300
					onTriggered: {
                        var searchResult = controller.blockManager().blockList().getSearchResult(searchField.displayText)
						searchResult = JSON.parse(searchResult)
						blockListContainer.showBlockModel(searchResult, false)
					}
				}
			}
        }
		IconFontButton {
			width: 30*dp
			iconSymbol: ""
			onPress: {
				searchField.text = ""
			}
		}
	}

	Item {  // spacer between serch field and list
        visible: !showCombinations
		height: 10*dp
	}

    // -------------- Blocks Tree View ----------

	Flickable {
		implicitHeight: -1  // stretch to fill height
		contentHeight: blockListContainer.implicitHeight + 20*dp
        clip: true
        visible: !showCombinations

		StretchColumn {
			id: blockListContainer
			height: implicitHeight
			width: parent.width
			defaultSize: 30*dp

			Component.onCompleted: {
				showAllBlocks()
			}

            Connections {
                target: controller
                onDeveloperModeChanged: blockListContainer.showAllBlocks()
            }

			function showAllBlocks() {
                var blockModel = JSON.parse(controller.blockManager().blockList().getJsonBlockModel())
				showBlockModel(blockModel, true)
			}

			function showBlockModel(model, collapsed) {
				clear()
				for (var i = 0; i < model.length; ++i) {
					var info = model[i]
					var level = 0
					addBlock(info, level, collapsed)
				}
			}

			function clear() {
				// TODO: does this properly destroy the children?
				children = null
//				var items = children
//				for (var i = items.length - 1; i >= 0; --i) {
//					items[i].destroy()
//				}
			}

			function addBlock(info, level, collapsed) {
				// check if block belongs to a subcategory:
				if (info.category.length > level) {
					var category = getCategory(info, level, collapsed)
					category.addBlock(info, level + 1, collapsed)
				} else {
					var entryComponent = Qt.createComponent("qrc:/qml/BlockListEntry.qml");
					var entryObject = entryComponent.createObject(blockListContainer, {blockInfo: info})
				}
			}

			function getCategory(info, level, collapsed) {
				// try to find category in existing items:
				var items = children
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
							blockListContainer, {name: name, level: level, collapsed: collapsed})
				return categoryObject
			}
		}
	}

    // --------------------- Combinations ------------------------

    Text {
        visible: showCombinations
        text: "Save all blocks in current\ngroup as combination:"
        font.pixelSize: 14*dp
        color: "#aaa"
        horizontalAlignment: Text.AlignHCenter
        height: 50*dp
    }

    BlockRow {
        height: 30*dp
        implicitHeight: 0  // do not stretch
        leftMargin: 10*dp
        visible: showCombinations
        Item {
            implicitWidth: -1
            TextInput {
                id: combinationNameInput
                anchors.fill: parent
                hintText: "New Combination"
                onAccepted: saveCombination()
            }
        }
        IconButton {
            width: 30*dp
            height: 30*dp
            iconName: "plus_icon"
            onPress: saveCombination()
        }
    }

    Item {
        visible: showCombinations
        height: 30*dp

        Rectangle {
            anchors.centerIn: parent
            height: 1
            width: parent.width
            color: "#aaa"
        }
    }

    ListView {
        id: listView
        implicitHeight: -1  // stretch to fill height
        clip: true
        visible: showCombinations
        model: controller.projectManager().combinations

        delegate: CombinationEntry {
            width: listView.width
        }
    }

}

