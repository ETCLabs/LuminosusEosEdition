import QtQuick 2.5
import CustomElements 1.0
import "../CustomBasics"
import "../CustomControls"
import "../."  // to import EosCueListEntry

BlockBase {
    id: root
    width: 540*dp
    height: 500*dp
    settingsComponent: settings

    // ------------------- Cue Update Logic ------------------------

    // this variable stores a pointer to the current displayed cueList
    property QtObject cueList

    function updateCueList() {
        cueList = controller.cueListManager().getCueList(block.cueListNumber)
        updateCues()
    }

    function updateCues() {
        if (cueList) cueListView.model = cueList.getCues()
    }

    function scrollToActive() {
        if (!cueList) return;
        var index = cueList.getActiveCueIndex()
        cueListView.positionViewAtIndex(index, ListView.Center)
    }


    // Case: cueListNumber in block changes:
    Connections {
        target: block
        onCueListNumberChanged: updateCueList()
    }

    // Case: cueLists in CueManager change:
    Connections {
        target: controller.cueListManager()
        onCueListsChanged: updateCueList()
    }

    // Case cues inside cueList change:
    Connections {
        target: cueList
        onCuesChanged: {
            updateCues()
            if (followCheckbox.active) scrollToActive()
        }
    }

    // Case active or pending cue info changes and follow mode is active:
    Connections {
        target: controller.eosManager()
        onCueInfoChanged: if (followCheckbox.active) scrollToActive()
    }

    // get initial values at component creation:
    Component.onCompleted: updateCueList()

    // ----------------------------------------------------------------

    StretchColumn {
        anchors.fill: parent

        Rectangle {
            height: 30*dp
            color: "#333"
            BlockRow {
                anchors.fill: parent
                Item {
                    width: 15*dp
                }
                Text {
                    width: 90*dp
                    text: "Cue"
                }
                StretchText {
                    text:  "Label"
                }
                Text {
                    width: 60*dp
                    text: "Int Up"
                }
                Text {
                    width: 80*dp
                    text: "Int Down"
                }
                Item {
                    width: 70*dp
                }
                HeightResizeArea {
                    width: 30*dp
                    target: root
                    minSize: 300*dp
                    maxSize: 1000*dp
                }
            }
        }


        Item {
            id: cuesColumn
            implicitHeight: -1

            ListView {
                id: cueListView
                anchors.fill: parent
                model: 0  // is set by Component.onComplete() of root
                clip: true

                delegate: EosCueListEntry {}

                Component.onCompleted: scrollToActive()
            }

            StretchText {
                anchors.fill: parent
                text: "Cue List is empty, doesn't exist or is still loading."
                hAlign: Text.AlignHCenter
                visible: cueListView.count === 0
            }
        }

        DragArea {
            implicitWidth: -1
            text: "Cue List " + block.cueListNumber + ": " + (cueList ? cueList.label : "")

            Text {
                text: "Follow"
                width: 55*dp
                height: 30*dp
                anchors.right: followCheckbox.left
            }

            CheckBox {
                id: followCheckbox
                width: 30*dp
                height: 30*dp
                anchors.right: resizeArea.left
                light: true
                active: true
                onActiveChanged: {
                    if (active) {
                        scrollToActive()
                    }
                }
            }

            WidthResizeArea {
                id: resizeArea
                target: root
                minSize: 420*dp
                maxSize: 1140*dp
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
                    text: "Cue List:"
                    width: parent.width - 40*dp
                }
                NumericInput {
                    width: 40*dp
                    minimumValue: 1
                    maximumValue: 999
                    value: block.cueListNumber
                    onValueChanged: {
                        if (value !== block.cueListNumber) {
                            block.cueListNumber = value
                        }
                    }
                }
            }
        }
    }  // end Settings Component
}
