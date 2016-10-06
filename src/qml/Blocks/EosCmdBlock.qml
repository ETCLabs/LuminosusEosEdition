import QtQuick 2.5
import CustomElements 1.0
import "../CustomBasics"
import "../CustomControls"

BlockBase {
    id: root
    width: Math.max(250*dp, Math.min(cmdTextLine.implicitWidth + 20*dp, 700*dp))
    height: 60*dp
    onWidthChanged: block.positionChanged()
    settingsComponent: settings

    StretchColumn {
        anchors.fill: parent

        BlockRow {
            Flickable {
                anchors.fill: parent
                anchors.leftMargin: 4*dp
                clip: true
                contentWidth: cmdTextLine.width
                contentHeight: cmdTextLine.height

                Text {
                    id: cmdTextLine
                    height: 30*dp
                    width: implicitWidth


                    // color depending on LIVE state
                    // check if string starts with the word "LIVE":
                    color: text.lastIndexOf("LIVE", 0) === 0 ?  "#EB9A14" : "#7598FF"
                    font.pixelSize: 18*dp

                    text: controller.eosManager().getCmdText(block.userId)
                    Connections {
                        target: controller.eosManager()
                        onCmdTextChanged: cmdTextLine.text = controller.eosManager().getCmdText(block.userId)
                    }
                    Connections {
                        target: block
                        onUserIdChanged: cmdTextLine.text = controller.eosManager().getCmdText(block.userId)
                    }
                }
            }
        }

        DragArea {
            text: "Cmd - User " + block.userId
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
                    text: "User:"
                    width: parent.width - 40*dp
                }
                NumericInput {
                    width: 40*dp
                    minimumValue: 0
                    maximumValue: 999
                    value: block.userId
                    onValueChanged: {
                        if (value !== block.userId) {
                            block.userId = value
                        }
                    }
                }
            }
        }
    }  // end Settings Component
}
