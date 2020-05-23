import QtQuick 2.5
import CustomElements 1.0
import "../../CustomBasics"
import "../../CustomControls"
import "../../."  // to import EosFaderItem

BlockBase {
    id: root
    width: (block.attr("numFaders").val)*90*dp
    height: 400*dp
    settingsComponent: settings


    StretchColumn {
        anchors.fill: parent

        StretchRow {
            implicitHeight: -1

            Repeater {
                id: faderRepeater
                model: block.attr("numFaders").val

                EosFaderItem {
                    index: modelData
                    feedbackEnabled: block.attr("feedbackEnabled").val
                }
            }
        }

        DragArea {
            text: "Fader Bank"

            StretchRow {
                width: 180*dp
                height: 30*dp
                anchors.right: parent.right
                anchors.rightMargin: 15*dp

                ButtonBottomLine {
                    width: 80*dp
                    text: "Page:"
                    onActiveChanged: {
                        if (active) {
                            block.attr("pageChangeMode").val = true
                        } else {
                            block.attr("pageChangeMode").val = false
                        }
                    }
                    mappingID: block.getUid() + "pageChange"
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

            OutputNode {
                node: block.node("outputNode")
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
                    text: "Catch external faders:"
                    width: parent.width - 30*dp
                }
                AttributeCheckbox {
                    width: 30*dp
                    attr: block.attr("catchFaders")
                }
            }
            BlockRow {
                StretchText {
                    text: "Faders count:"
                }
                AttributeNumericInput {
                    width:  55*dp
                    implicitWidth: 0
                    attr: block.attr("numFaders")
                }
            }
            BlockRow {
                Text {
                    text: "Feedback enabled:"
                    width: parent.width - 30*dp
                }
                AttributeCheckbox {
                    width: 30*dp
                    attr: block.attr("feedbackEnabled")
                }
            }
        }
    }  // end Settings Component
}
