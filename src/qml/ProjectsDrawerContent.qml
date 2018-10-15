import QtQuick 2.5
import CustomElements 1.0

import "CustomControls"
import "CustomBasics"


VerticalScrollView {
	contentItem: root

	StretchColumn {
		id: root
        anchors.fill: parent
        anchors.topMargin: 10*dp
        anchors.leftMargin: 15*dp
        anchors.rightMargin: 15*dp

        Text {
            height: 30*dp
            text: "Current Project:"
            color: "#fff"
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 18*dp
        }
        Text {
            height: 40*dp
            text: controller.projectManager().currentProjectName
            color: "#fff"
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignTop
            font.pixelSize: 22*dp
        }

        BlockRow {
            id: newAndCopyButtons
            height: 30*dp
            implicitHeight: 0  // do not stretch
            ButtonBottomLine {
                text: "Create New"
                onClick: {
                    newAndCopyButtons.visible = false
                    newProjectField.visible = true
                    projectName.forceActiveFocus()
                }
            }

            ButtonBottomLine {
                text: "Clone"
                onClick: {
                    newAndCopyButtons.visible = false
                    cloneProjectField.visible = true
                    projectCopyName.forceActiveFocus()
                }
            }
        }

        BlockRow {
            id: newProjectField
            height: 30*dp
            implicitHeight: 0  // do not stretch
            visible: false
            TextInput {
                id: projectName
                text: ""
                hintText: "New Project Name"
                width: parent.width - 30*dp
                height: parent.height
                inputMethodHints: Qt.ImhPreferLatin

                Keys.onEnterPressed: newProjectField.submit()
                Keys.onReturnPressed: newProjectField.submit()
                Keys.onEscapePressed: newProjectField.cancel()
            }
            IconButton {
                width: 30*dp
                height: 30*dp
                iconName: "plus_icon"
                onPress: newProjectField.submit()
            }

            function submit() {
                if (!projectName.displayText) return
                controller.projectManager().createAndLoad(projectName.displayText)
                projectName.text = ""
                newProjectField.visible = false
                newAndCopyButtons.visible = true
            }

            function cancel() {
                projectName.text = ""
                newProjectField.visible = false
                newAndCopyButtons.visible = true
            }
        }

        BlockRow {
            id: cloneProjectField
            height: 30*dp
            implicitHeight: 0  // do not stretch
            visible: false
            TextInput {
                id: projectCopyName
                text: ""
                hintText: "Name of Copy"
                width: parent.width - 30*dp
                height: parent.height
                inputMethodHints: Qt.ImhPreferLatin

                Keys.onEnterPressed: cloneProjectField.submit()
                Keys.onReturnPressed: cloneProjectField.submit()
                Keys.onEscapePressed: cloneProjectField.cancel()
            }
            IconButton {
                width: 30*dp
                height: 30*dp
                iconName: "copy_icon"
                onPress: cloneProjectField.submit()
            }

            function submit() {
                if (!projectCopyName.displayText) return
                controller.projectManager().copyCurrentAndLoad(projectCopyName.displayText)
                projectCopyName.text = ""
                cloneProjectField.visible = false
                newAndCopyButtons.visible = true
            }

            function cancel() {
                projectCopyName.text = ""
                cloneProjectField.visible = false
                newAndCopyButtons.visible = true
            }
        }

        BlockRow {
            height: 30*dp
            implicitHeight: 0  // do not stretch
            ButtonBottomLine {
                text: "Import"
                onClick: {
                    importDialogLoader.source = ""
                    importDialogLoader.source = "qrc:/qml/ImportProjectDialog.qml"
                }

                Loader {
                    id: importDialogLoader
                }
            }

            ButtonBottomLine {
                text: "Export"
                onClick: {
                    exportDialogLoader.source = ""
                    exportDialogLoader.source = "qrc:/qml/ExportProjectDialog.qml"
                }

                Loader {
                    id: exportDialogLoader
                }
            }

            ButtonBottomLine {
                text: "Clear"
                clickDurationEnabled: true
                onShortClick: guiManager.showToast("Long Click to Delete All Blocks")
                onLongClick: controller.blockManager().deleteAllBlocks()
            }
        }

		Spacer {
            height: 20*dp
		}

        ListView {
            width: parent.width
            implicitHeight: -1  // stretch
            model: controller.projectManager().projectList
            // prevent async buffering that can cause segfaults:
            cacheBuffer: 0
            clip: controller.output().luminosusInstances.length > 0
			delegate: Row {
				width: parent.width
				height: 30*dp
				ButtonSideLine {
					id: button
					width: parent.width - 30*dp
					height: 30*dp
					text: modelData
                    onClick: controller.projectManager().setCurrentProject(modelData)
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
                    clickDurationEnabled: true
                    onShortClick: guiManager.showToast("Long Click to Delete Project")
                    onLongClick: controller.projectManager().deleteProject(modelData)
				}

			}
        }
	}  // end main Column
}  // end ScrollView

