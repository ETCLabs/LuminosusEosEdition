import QtQuick 2.5
import CustomElements 1.0
import "../CustomBasics"
import "../CustomControls"

BlockBase {
    id: root
    width: 130*dp
    height: 90*dp

    StretchColumn {
        anchors.fill: parent

        BlockRow {
            InputNode {
                objectName: "inputNode"
            }
            Text {
                width: 60*dp
                text: "Time:"
            }
            NumericInput {
                minimumValue: 0.05
                maximumValue: 999
                suffix: "s"
                decimals: 1
                value: block.stepTime
                onValueChanged: {
                    if (value !== block.stepTime) {
                        block.stepTime = value
                    }
                }
            }
        }
        ButtonBottomLine {
            text: "Sync"
            onPress: block.sync()
        }

        DragArea {
            text: "Sinus"

            OutputNode {
                objectName: "outputNode"
            }
        }
    }  // end main column
}

