import QtQuick 2.5
import CustomElements 1.0

import "../CustomControls"
import "../CustomBasics"


StretchColumn {
    leftMargin: 15*dp
    rightMargin: 15*dp
    defaultSize: 30*dp
    height: implicitHeight

    BlockRow {
        visible: Qt.platform.os !== "ios" && Qt.platform.os !== "android"
        StretchText {
            text: "Fullscreen:"
            vAlign: Text.AlignVCenter
        }
        Switch {
            width: 90*dp
            implicitWidth: 0
            onActiveChanged: {
                active ? guiManager.getMainWindow().showFullScreen() : guiManager.getMainWindow().showNormal()
            }
        }
    }
    BlockRow {
        Text {
            text: "Snap to Grid:"
            width: parent.width - 30*dp
        }
        CheckBox {
            width: 30*dp
            active: guiManager.snapToGrid
            onActiveChanged: {
                if (active !== guiManager.snapToGrid) {
                    guiManager.snapToGrid = active
                }
            }
        }
    }
    BlockRow {
        Text {
            text: "Click Sounds:"
            width: parent.width - 30*dp
        }
        CheckBox {
            width: 30*dp
            active: controller.clickSounds
            onActiveChanged: {
                if (active !== controller.clickSounds) {
                    controller.clickSounds = active
                }
            }
        }
    }
    BlockRow {
        Text {
            text: "Background:"
            width: parent.width * 0.4
            verticalAlignment: Text.AlignVCenter
        }
        ComboBox2 {
            width: parent.width * 0.6
            values: ["blueprint_grey_tiled.jpg", "blueprint_tiled.jpg", "polygon_grey.jpg", "solid_grey.jpg", "gradient_grey.jpg", "solid_black.jpg"]
            texts: ["Greyprint", "Blueprint", "Polygon", "Grey", "Gradient", "Black"]
            property bool initialized: false
            Component.onCompleted: {
                setValue(guiManager.getBackgroundName())
                initialized = true
            }
            onValueChanged: {
                if (!initialized) return
                if (value !== guiManager.getBackgroundName()) {
                    guiManager.setBackgroundName(value);
                }
            }
        }
    }
//			BlockRow {
//				Text {
//					text: "Engine"
//					width: parent.width / 2
//					verticalAlignment: Text.AlignVCenter
//				}
//				Switch {
//					height: parent.height
//					width: parent.width / 2
//					active: true
//					onActiveChanged: {
//						active ? controller.engine().start() : controller.engine().stop()
//					}
//				}
//			}
    BlockRow {
        Text {
            text: "Auto Scaling:"
            width: parent.width - 30*dp
            color: "#aaa"
        }
        CheckBox {
            width: 30*dp
            active: !guiManager.overrideGuiScaling
            onActiveChanged: {
                if ((!active) !== guiManager.overrideGuiScaling) {
                    guiManager.overrideGuiScaling = !active
                }
            }
        }
    }
    BlockRow {
        visible: guiManager.overrideGuiScaling
        Text {
            text: "Scaling:"
            width: parent.width / 2
            verticalAlignment: Text.AlignVCenter
            color: "#aaa"
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
                    guiManager.setGuiScaling(value)
                }
            }
        }
    }
    BlockRow {
        Text {
            text: "Auto Graphics Level:"
            width: parent.width - 30*dp
            color: "#aaa"
        }
        CheckBox {
            width: 30*dp
            active: !guiManager.overrideGraphicsLevel
            onActiveChanged: {
                if ((!active) !== guiManager.overrideGraphicsLevel) {
                    guiManager.overrideGraphicsLevel = !active
                }
            }
        }
    }
    BlockRow {
        visible: guiManager.overrideGraphicsLevel
        Text {
            text: "Graphics:"
            width: parent.width / 2
            verticalAlignment: Text.AlignVCenter
            color: "#aaa"
        }
        ComboBox2 {
            width: parent.width / 2
            values: [1, 2, 3]
            texts: ["Low", "Mid", "High"]
            Component.onCompleted: setValue(GRAPHICAL_EFFECTS_LEVEL)
            onValueChanged: {
                if (value !== GRAPHICAL_EFFECTS_LEVEL) {
                    guiManager.setGraphicsLevel(value)
                }
            }
        }
    }
    BlockRow {
        Text {
            text: Qt.platform.os === "ios" ? "Expert Mode:" : "Developer Mode:"
            width: parent.width - 30*dp
            color: "#aaa"
        }
        CheckBox {
            width: 30*dp
            active: controller.developerMode
            onActiveChanged: {
                if (active !== controller.developerMode) {
                    controller.developerMode = active
                }
            }
        }
    }
}  // end App Settings column
