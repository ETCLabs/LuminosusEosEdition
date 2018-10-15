import QtQuick 2.5
import "."

NumericInput {
    property QtObject attr
    value: attr.val
    minimumValue: attr.min
    maximumValue: attr.max
    onValueChanged: {
        if (value !== attr.val) {
            attr.val = value
        }
    }
}
