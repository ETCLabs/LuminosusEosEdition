import QtQuick 2.5
import CustomElements 1.0
import "../../CustomBasics"
import "../../CustomControls"

BlockBase {
    id: root
    width: 70*dp
    // block is 8*dp smaller that button to end before the rounding of the edges starts
    height: width + 30*dp - 8*dp
    settingsComponent: settings

    StretchColumn {
        y: -8*dp
        height: parent.height + 8*dp
        width: parent.width

        PushButton {
            mappingID: block.getUid() + "launchpadbutton"
            toggle: block.attr("toggleMode").val
            text: block.attr("labelText").val
			onActiveChanged: root.block.value = (active ? 1 : 0)
        }

        DragArea {
            WidthResizeAreaLeft {
                target: root
                minSize: 60*dp
                maxSize: 200*dp
            }

            OutputNode {
                node: block.node("outputNode")
            }
        }
    }

    // -------------------------- Settings ----------------------------

    Component {
        id: settings
        StretchColumn {
            leftMargin: 15*dp
            rightMargin: 15*dp
            defaultSize: 30*dp

            BlockRow {
                Text {
                    text: "Toggle Mode:"
                    width: parent.width - 30*dp
                }
                AttributeCheckbox {
                    id: toggleCheckbox
                    attr: block.attr("toggleMode")
                    width: 30*dp
                }
            }

            BlockRow {
                StretchText {
                    text: "Low Value:"
                }
                NumericInput {
                    width: 120*dp
                    implicitWidth: 0
                    minimumValue: 1
                    maximumValue: 127
                    value: 0
                    decimals: 0
                    //value: root.block.bpm
                }
            }

            BlockRow {
                StretchText {
                    text: "High Value:"
                }
                NumericInput {
                    width: 120*dp
                    implicitWidth: 0
                    minimumValue: 1
                    maximumValue: 127
                    value: 127
                    decimals: 0
                    //value: root.block.bpm
                }
            }

            BlockRow {
                StretchText {
                    text: "Button Label:"
                }
                TextInput {
                    width: parent.width * 0.5
                    text: block.attr("labelText").val
                    onDisplayTextChanged: {
                        if (text !== block.attr("labelText").val) {
                            block.attr("labelText").val = text
                        }
                    }
                }
            }
        }
    }  // end Settings Component
}