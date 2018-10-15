import QtQuick 2.5
import CustomElements 1.0
import "../../CustomBasics"
import "../../CustomControls"

BlockBase {
    id: root
    width: 70*dp
    height: 200*dp
    settingsComponent: settings

    StretchColumn {
        anchors.fill: parent

        HeightResizeArea {
            height: 20*dp
            target: root
            minSize: 180*dp
            maxSize: 600*dp
        }

        NumericInput {
            centered: true
            minimumValue: 1
            maximumValue: 9999
            value: block.subNumber
            onValueChanged: {
                if (value !== block.subNumber) {
                    block.subNumber = value
                }
            }
        }

        Slider {
            id: slider
            mappingID: block.getUid() + "slider"
            value: block.value
            onValueChanged: {
                if (value !== block.value) {
                    block.value = value;
                }
            }
        }

        ButtonBottomLine {
            implicitHeight: 0  // do not stretch
            height: 30*dp
            text: "Bump"
            onActiveChanged: block.sendBumpEvent(active ? 1 : 0)
            mappingID: block.getUid() + "bump"
        }

        DragArea {
            text: "Sub"

            InputNode {
                node: block.node("inputNode")
            }
        }
    }  // end main Column

    Component {
        id: settings
        StretchColumn {
            leftMargin: 15*dp
            rightMargin: 15*dp
            defaultSize: 30*dp

            BlockRow {
                Text {
                    text: "Channel Number:"
                    width: parent.width - 40*dp
                }
                NumericInput {
                    width: 40*dp
                    minimumValue: 1
                    maximumValue: 9999
                    value: block.subNumber
                    onValueChanged: {
                        if (value !== block.subNumber) {
                            block.subNumber = value
                        }
                    }
                }
            }
        }
    }  // end Settings Component
}
