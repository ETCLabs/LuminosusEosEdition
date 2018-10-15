import QtQuick 2.5
import CustomElements 1.0

import "../../CustomBasics"
import "../../CustomControls"


BlockBase {
    id: root
    width: 230*dp
    height: 100*dp

    StretchColumn {
        anchors.fill: parent

        BlockRow {
            ButtonBottomLine {
                text: "Pause & Reset Lists"
                onClick: block.clearLists()
            }
        }
        BlockRow {
            ButtonBottomLine {
                text: "Clear Working Benches"
                onClick: block.clearBenches()
            }
        }

        DragArea {
            text: "Cue List Helper"
        }
    }
}

