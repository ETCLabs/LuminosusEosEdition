import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import CustomElements 1.0

import "CustomControls"
import "CustomBasics"

Rectangle {
    color: Qt.rgba(0, 0, 0, 0)

    BorderImage {
        anchors.fill: parent
        anchors.margins: -16
        border { left: 16; top: 16; right: 16; bottom: 16 }
        source: "qrc:/images/shadow_border.png"
		visible: GRAPHICAL_EFFECTS_LEVEL >=2
    }
    Rectangle {  // left border
        width: 1
        height: parent.height
        color: "#aaa"
    }

    Item {
        id: tabs
        width: parent.width
        height: 40*dp
		StretchRow {
			anchors.fill: parent
			DrawerTabButton {
				iconName: "add_icon"
				onPress: { tabView.currentIndex = 0 }
				marked: tabView.currentIndex === 0
			}
			DrawerTabButton {
				iconName: "settings_slider_icon"
				onPress: { tabView.currentIndex = 1 }
				marked: tabView.currentIndex === 1
			}
			DrawerTabButton {
				iconName: "save_icon"
				onPress: { tabView.currentIndex = 2 }
				marked: tabView.currentIndex === 2
			}
			DrawerTabButton {
				iconName: "gear_icon"
				onPress: { tabView.currentIndex = 3 }
				marked: tabView.currentIndex === 3
			}
		}
        Rectangle {  // line under tabs
            height: 1*dp
            y: parent.height
            width: parent.width
            color: "#aaa"
        }
    }

	TabView {
		id: tabView
		width: parent.width
		height: parent.height - tabs.height - 10*dp
		y: tabs.height + 10*dp
		tabsVisible: false
		style: TabViewStyle {
					 frame: Rectangle { color: "transparent" }
				 }
		currentIndex: 0
		Tab {
			id: blocksTab
			title: "Blocks"
			BlockListDrawerContent {}
		}
		Tab {
			id: blockSettingsTab
			title: "Block Settings"
			BlockSettingsDrawerContent {}
		}
		Tab {
			id: projectsTab
			title: "Projects"
			ProjectsDrawerContent {}
		}
		Tab {
			id: settingsTab
			title: "Settings"
			SettingsDrawerContent {}
		}
    }

    Connections {
        target: controller
        onOpenBlockSettings: {
            if (tabView.currentIndex !== 1) {
                tabView.currentIndex = 1
            }
        }
    }

}

