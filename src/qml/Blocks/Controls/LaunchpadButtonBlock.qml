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

        LaunchpadPushButton {
            mappingID: block.getUid() + "launchpadbutton"
            toggle: block.attr("toggleMode").val
            text: block.attr("labelText").val
			onActiveChanged: root.block.value = (active ? 1 : 0)
            low: block.attr("lowValue").val
            high: block.attr("highValue").val
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
                    minimumValue: 0
                    maximumValue: 127
                    decimals: 0
                    value: block.attr("lowValue").val
                    onValueChanged: {
                        if(value !== block.attr("lowValue").val) {
                            block.attr("lowValue").val = value
                        }
                    }
                }
            }

            BlockRow {
                StretchText {
                    text: "High Value:"
                }
                NumericInput {
                    width: 120*dp
                    implicitWidth: 0
                    minimumValue: 0
                    maximumValue: 127
                    decimals: 0
                    value: block.attr("highValue").val
                    onValueChanged: {
                        if(value !== block.attr("highValue").val) {
                            block.attr("highValue").val = value
                        }
                    }
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
