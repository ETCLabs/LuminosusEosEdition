import QtQuick 2.5
import CustomStyle 1.0
import CustomElements 1.0

import "CustomControls"
import "CustomBasics"

CustomTouchArea {
    implicitHeight: open ? 60*dp : 30*dp

    property bool open: false

    onClick: {
        if (open) {
            controller.playClickUpSound()
            open = false
        } else {
            controller.playClickSound()
            open = true
        }
    }

    StretchRow {  // row with left margin
        width: parent.width
        height: 30*dp
        leftMargin: 5*dp
        Item {  // arrow
            width: 30*dp
            Image {
                width: 10*dp
                height: 5*dp
                anchors.centerIn: parent
                source: "qrc:/images/down_arrow_slim.png"
                rotation: open ? 0 : -90
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
            width: parent.width - 35*dp
            verticalAlignment: Text.AlignVCenter
            fontSizeMode: Text.Fit
            text: modelData
        }
    } // end row with left margin

    StretchRow {
        width: parent.width
        height: 30*dp
        y: 30*dp
        leftMargin: 10*dp
        visible: open

        ButtonBottomLine {
            text: "Add to Workspace"
            onPress: controller.projectManager().addCombination(modelData)
            allUpperCase: false
        }
        IconButton {
            width: 30*dp
            height: 30*dp
            iconName: "trash_icon"
            clickDurationEnabled: true
            onShortClick: guiManager.showToast("Long Click to Delete Combination")
            onLongClick: controller.projectManager().removeCombination(modelData)
        }
    }
}
