#include "BlockList.h"

#include "block_implementations/Audio/AudioVolumeBlock.h"
#include "block_implementations/Audio/AudioPlaybackBlock.h"
#include "block_implementations/Audio/BpmBlock.h"
#include "block_implementations/Audio/BeatDetectionBlock.h"
#include "block_implementations/Audio/AudioLevelBlock.h"
#include "block_implementations/Audio/AudioStreamingBlock.h"
#include "block_implementations/Controls/SwitchBlock.h"
#include "block_implementations/Controls/ButtonBlock.h"
#include "block_implementations/Controls/LaunchpadButtonBlock.h"
#include "block_implementations/Controls/SliderBlock.h"
#include "block_implementations/Controls/ColorwheelBlock.h"
#include "block_implementations/Controls/PixelControlBlock.h"
#include "block_implementations/Eos/EosMasterPlaybackBlock.h"
#include "block_implementations/Eos/EosCmdBlock.h"
#include "block_implementations/Eos/EosOscMonitorBlock.h"
#include "block_implementations/Eos/EosGroupBlock.h"
#include "block_implementations/Eos/EosCueBlock.h"
#include "block_implementations/Eos/EosEffectBlock.h"
#include "block_implementations/Eos/EosFaderBankBlock.h"
#include "block_implementations/Eos/EosWheelsBlock.h"
#include "block_implementations/Eos/EosKeyBlock.h"
#include "block_implementations/Eos/EosCueListBlock.h"
#include "block_implementations/Eos/EosSingleFaderBlock.h"
#include "block_implementations/Eos/EosMacroBlock.h"
#include "block_implementations/Eos/EosChanBlock.h"
#include "block_implementations/Eos/EosInfoBlock.h"
#include "block_implementations/Eos/EosSubBlock.h"
#include "block_implementations/Eos/EosEncoderBlock.h"
#include "block_implementations/Eos/EosSpeedMasterBlock.h"
#include "block_implementations/Theater/DimmerBlock.h"
#include "block_implementations/Theater/RgbLightBlock.h"
#include "block_implementations/Theater/RgbWLightBlock.h"
#include "block_implementations/Theater/RgbWAUVLightBlock.h"
#include "block_implementations/Theater/VerticalPatchBlock.h"
#include "block_implementations/Theater/PresetBlock.h"
#include "block_implementations/Theater/SceneSliderBlock.h"
#include "block_implementations/Theater/CueListBlock.h"
#include "block_implementations/Theater/CueListHelperBlock.h"
#include "block_implementations/Theater/MatrixBlock.h"
#include "block_implementations/Info/StopwatchBlock.h"
#include "block_implementations/Info/PreviewBlock.h"
#include "block_implementations/Info/QmlOnlyBlock.h"
#include "block_implementations/Info/ClockBlock.h"
#include "block_implementations/Info/NotesBlock.h"
#include "block_implementations/Info/ImageBlock.h"
#include "block_implementations/Info/VideoBlock.h"
#include "block_implementations/Info/StyledTextBlock.h"
#include "block_implementations/Logic/DecayBlock.h"
#include "block_implementations/Logic/DelayBlock.h"
#include "block_implementations/Logic/ScriptBlock.h"
#include "block_implementations/Logic/RandomValueBlock.h"
#include "block_implementations/Logic/TickGeneratorBlock.h"
#include "block_implementations/Logic/ThresholdBlock.h"
#include "block_implementations/Logic/SinusValueBlock.h"
#include "block_implementations/Logic/HsvBlock.h"
#include "block_implementations/Logic/LinearValueBlock.h"
#include "block_implementations/Logic/OneDimensionalPattern.h"
#include "block_implementations/Logic/SmoothBlock.h"
#include "block_implementations/Logic/HoldMaxBlock.h"
#include "block_implementations/Logic/ValueProjectionBlock.h"
#include "block_implementations/Logic/BasicTransformBlock.h"
#include "block_implementations/Logic/ColorizeBlock.h"
#include "block_implementations/Logic/CrossfadeBlock.h"
#include "block_implementations/Logic/MovingPatternBlock.h"
#include "block_implementations/Logic/OffsetBlock.h"
#include "block_implementations/Logic/SequencerBlock.h"
#include "block_implementations/Logic/MultiplyBlock.h"
#include "block_implementations/Logic/RecorderBlock.h"
#include "block_implementations/Logic/RecorderSlaveBlock.h"
#include "block_implementations/Logic/ProjectorShutterBlock.h"
#include "block_implementations/Logic/ToggleBlock.h"
#include "block_implementations/Logic/CommandlineBlock.h"
#include "block_implementations/Logic/KeyPressBlock.h"
#include "block_implementations/Logic/PresentationSlideBlock.h"
#include "block_implementations/Logic/PresentationSlideDmxBlock.h"
#include "block_implementations/Logic/PresentationRemoteBlock.h"
#include "block_implementations/Logic/DmxValueRangeBlock.h"
#include "block_implementations/Luminosus/LuminosusInfoBlock.h"
#include "block_implementations/Luminosus/ControlsTestBlock.h"
#include "block_implementations/Luminosus/ChangeProjectBlock.h"
#include "block_implementations/Luminosus/LogBlock.h"
#include "block_implementations/Luminosus/PageAnchorBlock.h"
#include "block_implementations/Luminosus/DebugBlock.h"
#include "block_implementations/Luminosus/GuiEditorBlock.h"
#include "block_implementations/Luminosus/GroupBlock.h"
#include "block_implementations/MIDI/MidiNoteInBlock.h"
#include "block_implementations/MIDI/MidiNoteInRangeBlock.h"
#include "block_implementations/MIDI/MidiNoteOutBlock.h"
#include "block_implementations/MIDI/MidiProgramInBlock.h"
#include "block_implementations/MIDI/MidiProgramOutBlock.h"
#include "block_implementations/MIDI/MidiControlInBlock.h"
#include "block_implementations/MIDI/MidiControlOutBlock.h"
#include "block_implementations/MIDI/MidiMonitorBlock.h"
#include "block_implementations/OSC/OscInBlock.h"
#include "block_implementations/OSC/OscOutBlock.h"
#include "block_implementations/OSC/OscMonitorBlock.h"
#include "block_implementations/X32/X32ChannelBlock.h"
#include "block_implementations/X32/XAirAuxBlock.h"
#include "block_implementations/X32/X32OscMonitorBlock.h"
#include "block_implementations/sACN/SAcnInBlock.h"
#include "block_implementations/sACN/VirtualFixtureBlock.h"
#include "block_implementations/Hog4/Hog4FaderBlock.h"

void BlockList::addAllBlocks() {
    // defines order in UI

    // --- General ---

    // Controls
    addBlock(SliderBlock::info());
    addBlock(ButtonBlock::info());
    addBlock(LaunchpadButtonBlock::info());
    addBlock(SwitchBlock::info());
    addBlock(PixelControlBlock::info());
    addBlock(ColorwheelBlock::info());
    addBlock(HsvBlock::info());

    // Modify Value
    addBlock(ThresholdBlock::info());
    addBlock(SmoothBlock::info());
    addBlock(MultiplyBlock::info());
    addBlock(DelayBlock::info());
    addBlock(DecayBlock::info());
    addBlock(HoldMaxBlock::info());
    addBlock(ValueProjectionBlock::info());

    // Logic
    addBlock(CrossfadeBlock::info());
    addBlock(SequencerBlock::info());
    addBlock(ToggleBlock::info());
    addBlock(ScriptBlock::info());
    addBlock(RecorderBlock::info());
    addBlock(RecorderSlaveBlock::info());
    addBlock(CommandlineBlock::info());

    // Generator
    addBlock(LinearValueBlock::info());
    addBlock(SinusValueBlock::info());
    addBlock(RandomValueBlock::info());
    addBlock(TickGeneratorBlock::info());

    // Line / Matrix:
    addBlock(VerticalPatchBlock::info());
    addBlock(BasicTransformBlock::info());
    addBlock(OffsetBlock::info());
    addBlock(OneDimensionalPattern::info());
    addBlock(ColorizeBlock::info());

    // Media Playback
    addBlock(AudioPlaybackBlock::info());
    addBlock(AudioStreamingBlock::info());
    addBlock(ImageBlock::info());
    addBlock(VideoBlock::info());
    addBlock(PresentationSlideBlock::info());
    addBlock(PresentationSlideDmxBlock::info());
    addBlock(PresentationRemoteBlock::info());
    addBlock(ProjectorShutterBlock::info());

    // sACN Input
    addBlock(SAcnInBlock::info());
    addBlock(VirtualFixtureBlock::info());
    addBlock(DmxValueRangeBlock::info());

    // Groups / Projects
    addBlock(GroupBlock::info());
    addBlock(ChangeProjectBlock::info());

    // Presentation
    addBlock(PageAnchorBlock::info());
    addBlock(StyledTextBlock::info());

    // Other
    addBlock(NotesBlock::info());
    addBlock(ClockBlock::info());
    addBlock(StopwatchBlock::info());
    addBlock(KeyPressBlock::info());
    addBlock(QmlOnlyBlock::info());

    // Debugging:
    addBlock(LogBlock::info());
    addBlock(DebugBlock::info());
    addBlock(ControlsTestBlock::info());
    addBlock(LuminosusInfoBlock::info());
    addBlock(GuiEditorBlock::info());

    addBlock(PreviewBlock::info());

    // ------------------

    // Eos
    addBlock(EosChanBlock::info());
    addBlock(EosGroupBlock::info());
    addBlock(EosSubBlock::info());
    addBlock(EosKeyBlock::info());
    addBlock(EosMacroBlock::info());
    addBlock(EosEffectBlock::info());
    addBlock(EosSpeedMasterBlock::info());
    addBlock(EosCueBlock::info());
    addBlock(EosCueListBlock::info());
    addBlock(EosSingleFaderBlock::info());
    addBlock(EosFaderBankBlock::info());
    addBlock(EosMasterPlaybackBlock::info());
    addBlock(EosEncoderBlock::info());
    addBlock(EosWheelsBlock::info());
    addBlock(EosCmdBlock::info());
    addBlock(EosInfoBlock::info());
    addBlock(EosOscMonitorBlock::info());

    // Hog 4
    addBlock(Hog4FaderBlock::info());

    // X32
    addBlock(X32ChannelBlock::info());
    addBlock(XAirAuxBlock::info());
    addBlock(X32OscMonitorBlock::info());

    // Sound2Light
    addBlock(AudioLevelBlock::info());
    addBlock(AudioVolumeBlock::info());
    addBlock(BeatDetectionBlock::info());
    addBlock(BpmBlock::info());

    // OSC
    addBlock(OscInBlock::info());
    addBlock(OscOutBlock::info());
    addBlock(OscMonitorBlock::info());

    // MIDI
    addBlock(MidiNoteInBlock::info());
    addBlock(MidiNoteInRangeBlock::info());
    addBlock(MidiNoteOutBlock::info());
    addBlock(MidiControlInBlock::info());
    addBlock(MidiControlOutBlock::info());
    addBlock(MidiProgramInBlock::info());
    addBlock(MidiProgramOutBlock::info());
    addBlock(MidiMonitorBlock::info());

    // Theater
    addBlock(DimmerBlock::info());
    addBlock(RgbLightBlock::info());
    addBlock(RgbWLightBlock::info());
    addBlock(RgbWAUVLightBlock::info());
    addBlock(PresetBlock::info());
    addBlock(CueListBlock::info());
    addBlock(CueListHelperBlock::info());
    addBlock(SceneSliderBlock::info());
    addBlock(MatrixBlock::info());
}
