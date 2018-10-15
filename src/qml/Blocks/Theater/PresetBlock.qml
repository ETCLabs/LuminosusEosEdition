import QtQuick 2.5
import CustomElements 1.0
import "../../CustomBasics"
import "../../CustomControls"

BlockBase {
    id: root
    width: 120*dp
    height: 260*dp
    settingsComponent: settings

    StretchColumn {
        anchors.fill: parent

        HeightResizeArea {
            height: 20*dp
            target: root
            minSize: 230*dp
            maxSize: 600*dp
        }

        AttributeSlider {
            mappingID: block.getUid() + "slider"
            attr: block.attr("value")
        }

        ButtonSideLine {
            implicitHeight: 0  // do not stretch
            height: 30*dp
            text: "from Benches"
            allUpperCase: false
            onPress: block.saveFromBenches()
            mappingID: block.getUid() + "save_from_benches"
        }
        ButtonSideLine {
            implicitHeight: 0  // do not stretch
            height: 30*dp
            text: "to Benches"
            allUpperCase: false
            onPress: block.pushToBanks()
            mappingID: block.getUid() + "push_to_benches"
        }
        ButtonSideLine {
            implicitHeight: 0  // do not stretch
            height: 30*dp
            text: "from Mix"
            allUpperCase: false
            onPress: block.saveFromMix()
            mappingID: block.getUid() + "save_from_mix"
        }
        ButtonSideLine {
            implicitHeight: 0  // do not stretch
            height: 30*dp
            text: "+ Cue"
            allUpperCase: false
            onPress: block.addToCueList()
            mappingID: block.getUid() + "add_as_cue"
        }

        DragArea {
            text: "Preset"
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
                    text: "Scene Group:"
                    width: parent.width - 40*dp
                }
                NumericInput {
                    width: 40*dp
                    minimumValue: 0
                    maximumValue: 9999
                    value: block.sceneGroup
                    onValueChanged: {
                        if (value !== block.sceneGroup) {
                            block.sceneGroup = value
                        }
                    }
                }
            }
        }
    }  // end Settings Component
}
