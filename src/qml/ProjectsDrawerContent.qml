import QtQuick 2.5
import Qt.labs.folderlistmodel 2.1
import CustomElements 1.0

import "CustomControls"
import "CustomBasics"


VerticalScrollView {
	contentItem: root

	StretchColumn {
		id: root
		width: parent.width
		leftMargin: 15*dp
		rightMargin: 15*dp
		defaultSize: 30*dp

		BlockRow {
			TextInput {
				id: projectName
				text: ""
				hintText: "Name"
				width: parent.width - 60*dp
				height: parent.height
				inputMethodHints: Qt.ImhPreferLatin

				Keys.onEnterPressed: {
					if (!projectName.text) return
					controller.projectManager().createAndLoad(projectName.displayText)
					projectName.text = ""
				}
				Keys.onReturnPressed: {
					if (!projectName.text) return
					controller.projectManager().createAndLoad(projectName.displayText)
					projectName.text = ""
				}
			}
			IconButton {
				width: 30*dp
				height: 30*dp
				iconName: "plus_icon"
				onPress: {
					if (!projectName.text) return
					controller.projectManager().createAndLoad(projectName.displayText)
					projectName.text = ""
				}
			}
			IconButton {
				width: 30*dp
				height: 30*dp
				iconName: "copy_icon"
				onPress: {
					if (!projectName.text) return
					controller.projectManager().copyCurrentAndLoad(projectName.displayText)
					projectName.text = ""
				}
			}
		}
		Spacer {
			height: 20*dp
		}

		ListView {
			width: parent.width
			height: count * 30*dp
			model: controller.projectManager().projectList
			delegate: Row {
				width: parent.width
				height: 30*dp
				ButtonSideLine {
					id: button
					width: parent.width - 30*dp
					height: 30*dp
					text: modelData
					onPress: controller.projectManager().setCurrentProject(modelData)
					marked: modelData === controller.projectManager().getCurrentProjectName()
					allUpperCase: false
					Connections {
						target: controller.projectManager()
						onProjectChanged: {
							button.marked = modelData === controller.projectManager().getCurrentProjectName()
						}
					}
					Rectangle {
						anchors.fill: parent
						color: "white"
						opacity: 0.1
						visible: button.marked
					}
				}
				IconButton {
					width: 30*dp
					height: 30*dp
					iconName: "trash_icon"
					onPress: controller.projectManager().deleteProject(modelData)
				}

			}
		}
	}  // end main Column
}  // end ScrollView

