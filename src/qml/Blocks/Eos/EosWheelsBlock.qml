import QtQuick 2.5
import CustomElements 1.0
import "../../CustomBasics"
import "../../CustomControls"

BlockBase {
    id: root
    width: 350*dp
    height: 60*dp + paramColumn.implicitHeight

    StretchColumn {
        anchors.fill: parent

        BlockRow {
            StretchText {
                id: label
                text: controller.activeChannelsManager().activeChannelsLabel
                font.bold: true
                visible: text !== ""
                hAlign: Text.AlignHCenter
            }
            StretchText {
                text: "No Channel selected."
                hAlign: Text.AlignHCenter
                visible: label.text === ""
            }
        }

        StretchColumn {
            id: paramColumn
            height: implicitHeight
            defaultSize: 30*dp
            leftMargin: 8*dp

            Repeater {
                model: controller.activeChannelsManager().wheelCount

                StretchRow {
                    height: 30*dp
                    StretchText {
                        text: controller.activeChannelsManager().wheelLabels[modelData]
                        hAlign: Text.AlignLeft
                    }
                    ButtonBottomLine {
                        text: "-"
                        width: 30*dp
                        implicitWidth: 0  // do not stretch
                        onActiveChanged: {
                            if (active) {
                                controller.activeChannelsManager().startChangeParameter(modelData, -0.2)
                            } else {
                                controller.activeChannelsManager().stopChangeParameter(modelData)
                            }
                        }
                        mappingID: block.getUid() + "-" + modelData
                    }
                    StretchText {
                        width: 35*dp
                        implicitWidth: 0  // do not stretch
                        text: controller.activeChannelsManager().wheelValues[modelData]
                        hAlign: Text.AlignHCenter
                    }
                    ButtonBottomLine {
                        text: "+"
                        width: 30*dp
                        implicitWidth: 0  // do not stretch
                        onActiveChanged: {
                            if (active) {
                                controller.activeChannelsManager().startChangeParameter(modelData, 0.2)
                            } else {
                                controller.activeChannelsManager().stopChangeParameter(modelData)
                            }
                        }
                        mappingID: block.getUid() + "+" + modelData
                    }
                }
            }  // end Repeater
        }

        DragArea {
            text: "Channel Parameters"
        }
    }  // end main Column
}
