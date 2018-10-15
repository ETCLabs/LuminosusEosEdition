import QtQuick 2.5
import CustomElements 1.0
import "../../CustomBasics"
import "../../CustomControls"

BlockBase {
    id: root
    width: 200*dp
    height: 60*dp
    settingsComponent: settings

    StretchColumn {
        anchors.fill: parent

        BlockRow {
            height: 30*dp

            ComboBox2 {
                values: controller.projectManager().projectList

                property bool initialized: false
                Component.onCompleted: {
                    setValue(block.projectName)
                    if (getValue() !== block.projectName) {
                        block.projectName = getValue();
                    }

                    initialized = true
                }
                onValueChanged: {
                    if (!initialized) return
                    if (value !== block.projectName) {
                        block.projectName = value;
                    }
                }
            }

            IconButton {
                width: 30*dp
                iconName: "fader_go_icon"
                onPress: block.fire()
                mappingID: block.getUid() + "fire"
            }
        }

        DragArea {
            text: "Project"

            InputNode {
                node: block.node("inputNode")
                suggestions: ["OSC In", "MIDI Control In", "MIDI Note In"]
            }
        }
    }

    Component {
        id: settings

        StretchColumn {
            leftMargin: 15*dp
            rightMargin: 15*dp
            defaultSize: 30*dp

            BlockRow {
                StretchText {
                    text: "Animated:"
                }
                CheckBox {
                    width: 30*dp
                    active: block.animated
                    onActiveChanged: {
                        if (active !== block.animated) {
                            block.animated = active
                        }
                    }
                }
            }
        }
    }  // end Settings Component
}

