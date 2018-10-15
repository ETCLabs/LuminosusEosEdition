import QtQuick 2.5
import "."

ComboBox2 {
    id: root
    property QtObject attr
    property bool initialized: false
    Component.onCompleted: {
        setValue(attr.val)
        initialized = true
    }
    onValueChanged: {
        if (!initialized) return
        if (value !== attr.val) {
            attr.val = value;
        }
    }
    Connections {
        target: attr
        onValueChanged: {
            if (!initialized) return
            if (root.getValue() !== attr.val) {
                root.setValue(attr.val);
            }
        }
    }
}
