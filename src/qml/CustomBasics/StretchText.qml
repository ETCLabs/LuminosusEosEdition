import QtQuick 2.5

import "./."  // to import custom Text

Item {
	property alias text: label.text
	property alias font: label.font
    property alias color: label.color
	property alias hAlign: label.horizontalAlignment
	property alias vAlign: label.verticalAlignment
    property alias textWidth: label.implicitWidth
    property alias textFormat: label.textFormat
    property alias elide: label.elide
	implicitHeight: -1
	implicitWidth: -1

	Text {
		id: label
		anchors.fill: parent
		horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
        fontSizeMode: Text.Fit
        elide: Text.ElideRight
	}

}
