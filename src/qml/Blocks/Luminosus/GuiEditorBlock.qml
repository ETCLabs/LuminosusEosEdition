import QtQuick 2.5
import CustomElements 1.0
import QtQuick.Controls 1.4
import "../../CustomBasics"
import "../../CustomControls"

BlockBase {
    id: root
    width: 700*dp
    height: 700*dp

    StretchColumn {
        anchors.fill: parent

        ScrollView {
            implicitHeight: -1
            TextEdit {
                id: codeInput
                objectName: "codeInput"
                height: implicitHeight
                width: implicitWidth
                //hintText: "v = "
                text: block.attr("code").val
                onTextChanged: {
                    if (text !== block.attr("code").val) {
                        block.attr("code").val = text
                    }
                }
                color: block.attr("saved").val ? "#fff" : "#faa"
                font.family: "Quicksand"
                font.pixelSize: 18*dp
                selectByMouse: true
            }
        }

        ButtonSideLine {
            height: 30*dp
            text: "Apply"
            onClick: block.apply()
        }

        DragArea {
            text: "GUI Editor"
        }
    }  // end main Column
}
