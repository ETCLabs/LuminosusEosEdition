import QtQuick 2.5
import CustomElements 1.0

import "../CustomControls"
import "../CustomBasics"


StretchColumn {
    id: root
    leftMargin: 15*dp
    rightMargin: 15*dp
    defaultSize: 30*dp
    height: implicitHeight

    property string mappingID: "drawer"

    StretchText {
        height: 120*dp
        implicitHeight: 0
        text: "How to map a MIDI controller to\nfaders or buttons:\n\n"
            + "1. Click 'Map MIDI -> Control' below\n"
            + "2. Touch a fader or button in Luminosus\n"
            + "3. Move a fader or push a button on the\nMIDI controller\n"
            + "(Touch sensitive faders are problematic.)"
    }

    BlockRow {
        ButtonSideLine {
            width: parent.width
            mappingID: root.mappingID + "midi_mapping_connect_with_feedback"
            text: "Map MIDI -> Control"
            onPress: {
                controller.midiMapping().startMappingMidiToControlWithFeedback()
                toast.displayToast("1. Touch GUI control\n2. Use MIDI control", false)
            }
        }
    }
    StretchText {
        height: 140*dp
        implicitHeight: 0
        text: "How to release a MIDI mapping:\n\n"
            + "- Click 'Release GUI' and touch a fader\n"
            + "  or button in Luminosus to remove\n"
            + "  its mapping\n"
            + "- Or click 'Release MIDI' and move a\n"
            + "  fader or push a button on the\n"
            + "  MIDI controller to remove its mapping"
    }
    BlockRow {
        ButtonSideLine {
            width: parent.width
            mappingID: root.mappingID + "midi_mapping_release_gui"
            text: "Release GUI"
            onPress: {
                controller.midiMapping().releaseNextControlMapping()
                toast.displayToast("Touch GUI control\nto release its mapping.", false)
            }
        }
    }
    BlockRow {
        ButtonSideLine {
            width: parent.width
            mappingID: root.mappingID + "midi_mapping_release_midi"
            text: "Release MIDI"
            onPress: {
                controller.midiMapping().releaseNextMidiEventMapping()
                toast.displayToast("Use MIDI control\nto release its mapping.", false)
            }
        }
    }
    BlockRow {
        Text {
            width: parent.width - 30*dp
            text: "Send Feedback:"
        }
        CheckBox {
            width: 30*dp
            mappingID: root.mappingID + "midi_mapping_feedback_enabled"
            active: controller.midiMapping().feedbackEnabled
            onActiveChanged: {
                if (active !== controller.midiMapping().feedbackEnabled) {
                    controller.midiMapping().feedbackEnabled = active
                }
            }
        }
    }
}  // MIDI column end
