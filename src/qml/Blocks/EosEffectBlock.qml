import QtQuick 2.5
import CustomElements 1.0
import "../CustomBasics"
import "../CustomControls"

BlockBase {
    id: root
    width: 140*dp
    height: 60*dp
    settingsComponent: settings

    StretchColumn {
        anchors.fill: parent

        NumericInput {
            centered: true
            minimumValue: 1
            maximumValue: 9999
            decimals: 2
            value: block.effectNumber
            onValueChanged: {
                if (value !== block.effectNumber) {
                    block.effectNumber = value
                }
            }
        }

        DragArea {
            text: "Effect Speed"
            InputNode {
                objectName: "inputNode"
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
                    text: "Effect Number:"
                    width: parent.width - 40*dp
                }
                NumericInput {
                    width: 40*dp
                    minimumValue: 1
                    maximumValue: 9999
                    decimals: 2
                    value: block.effectNumber
                    onValueChanged: {
                        if (value !== block.effectNumber) {
                            block.effectNumber = value
                        }
                    }
                }
            }
        }
    }  // end Settings Component
}

