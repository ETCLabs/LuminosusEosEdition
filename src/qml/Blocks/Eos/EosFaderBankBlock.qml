import QtQuick 2.5
import CustomElements 1.0
import "../../CustomBasics"
import "../../CustomControls"
import "../../."  // to import EosFaderItem

BlockBase {
    id: root
    width: 90*10*dp
    height: 400*dp

    StretchColumn {
        anchors.fill: parent

        StretchRow {
            implicitHeight: -1

            Repeater {
                model: 10

                EosFaderItem {
                    index: modelData
                }
            }
        }

        DragArea {
            text: "Fader Bank"

            StretchRow {
                width: 140*dp
                height: 30*dp
                anchors.right: parent.right
                Text {
                    width: 60*dp
                    text: "Page:"
                }
                ButtonBottomLine {
                    width: 30*dp
                    text: "-"
                    onPress: block.sendPageMinusEvent()
                    mappingID: block.getUid() + "pageMinus"
                }
                NumericInput {
                    width: 40*dp
                    minimumValue: 1
                    maximumValue: 30
                    value: block.page
                    onValueChanged: {
                        if (value !== block.page) {
                            block.page = value
                        }
                    }
                }
                ButtonBottomLine {
                    width: 30*dp
                    text: "+"
                    onPress: block.sendPagePlusEvent()
                    mappingID: block.getUid() + "pagePlus"
                }
            }
        }

    }  // end main Column
}
