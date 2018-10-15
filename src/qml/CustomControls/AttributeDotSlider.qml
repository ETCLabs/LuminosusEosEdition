import QtQuick 2.5
import "."

DotSlider {
    property QtObject attr
    value: attr.val
    onValueChanged: {
        if (value !== attr.val) {
            attr.val = value
        }
    }
    mappingID: attr.block().getUid() + attr.name()
}
