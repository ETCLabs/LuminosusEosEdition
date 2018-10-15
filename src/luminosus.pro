
# ------------------ General configurations -----------------------

TEMPLATE = app

QT += qml quick multimedia svg core-private networkauth

CONFIG += c++14

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

DEFINES += QT_MESSAGELOGCONTEXT

ICON = images/icon/app_icon_icns.icns

# Windows .exe icon:
RC_FILE = luminosus.rc


# ------------------ Resources (.cpp, .h, .qrc, icons) -----------------------

RESOURCES += qml.qrc \
    images.qrc \
    fonts.qrc \
    qmlBlocks.qrc \
    examples.qrc \
    sounds.qrc \
    tutorial.qrc

SOURCES += main.cpp \
    audio/AudioEngine.cpp \
    audio/AudioInputAnalyzer.cpp \
    audio/AudioPlayerQt.cpp \
    audio/AudioWaveform.cpp \
    audio/QWaveDecoder.cpp \
    audio/SpeechInputAnalyzer.cpp \
    block_implementations/Audio/AudioLevelBlock.cpp \
    block_implementations/Audio/AudioPlaybackBlock.cpp \
    block_implementations/Audio/AudioStreamingBlock.cpp \
    block_implementations/Audio/AudioVolumeBlock.cpp \
    block_implementations/Audio/BeatDetectionBlock.cpp \
    block_implementations/Audio/BpmBlock.cpp \
    block_implementations/Controls/ButtonBlock.cpp \
    block_implementations/Controls/ColorwheelBlock.cpp \
    block_implementations/Controls/PixelControlBlock.cpp \
    block_implementations/Controls/SliderBlock.cpp \
    block_implementations/Controls/SwitchBlock.cpp \
    block_implementations/Eos/EosChanBlock.cpp \
    block_implementations/Eos/EosCmdBlock.cpp \
    block_implementations/Eos/EosCueBlock.cpp \
    block_implementations/Eos/EosCueListBlock.cpp \
    block_implementations/Eos/EosEffectBlock.cpp \
    block_implementations/Eos/EosEncoderBlock.cpp \
    block_implementations/Eos/EosFaderBankBlock.cpp \
    block_implementations/Eos/EosGroupBlock.cpp \
    block_implementations/Eos/EosInfoBlock.cpp \
    block_implementations/Eos/EosKeyBlock.cpp \
    block_implementations/Eos/EosMacroBlock.cpp \
    block_implementations/Eos/EosMasterPlaybackBlock.cpp \
    block_implementations/Eos/EosOscMonitorBlock.cpp \
    block_implementations/Eos/EosSingleFaderBlock.cpp \
    block_implementations/Eos/EosSubBlock.cpp \
    block_implementations/Eos/EosWheelsBlock.cpp \
    block_implementations/Info/ClockBlock.cpp \
    block_implementations/Info/ImageBlock.cpp \
    block_implementations/Info/NotesBlock.cpp \
    block_implementations/Info/PreviewBlock.cpp \
    block_implementations/Info/QmlOnlyBlock.cpp \
    block_implementations/Info/StopwatchBlock.cpp \
    block_implementations/Info/StyledTextBlock.cpp \
    block_implementations/Info/VideoBlock.cpp \
    block_implementations/Logic/BasicTransformBlock.cpp \
    block_implementations/Logic/ColorizeBlock.cpp \
    block_implementations/Logic/CommandlineBlock.cpp \
    block_implementations/Logic/CrossfadeBlock.cpp \
    block_implementations/Logic/DecayBlock.cpp \
    block_implementations/Logic/DelayBlock.cpp \
    block_implementations/Logic/HoldMaxBlock.cpp \
    block_implementations/Logic/HsvBlock.cpp \
    block_implementations/Logic/LinearValueBlock.cpp \
    block_implementations/Logic/MovingPatternBlock.cpp \
    block_implementations/Logic/MultiplyBlock.cpp \
    block_implementations/Logic/OffsetBlock.cpp \
    block_implementations/Logic/OneDimensionalPattern.cpp \
    block_implementations/Logic/ProjectorShutterBlock.cpp \
    block_implementations/Logic/RandomValueBlock.cpp \
    block_implementations/Logic/RecorderBlock.cpp \
    block_implementations/Logic/RecorderSlaveBlock.cpp \
    block_implementations/Logic/ScriptBlock.cpp \
    block_implementations/Logic/SequencerBlock.cpp \
    block_implementations/Logic/SinusValueBlock.cpp \
    block_implementations/Logic/SmoothBlock.cpp \
    block_implementations/Logic/ThresholdBlock.cpp \
    block_implementations/Logic/TickGeneratorBlock.cpp \
    block_implementations/Logic/TimerBlock.cpp \
    block_implementations/Logic/ToggleBlock.cpp \
    block_implementations/Logic/ValueProjectionBlock.cpp \
    block_implementations/Luminosus/ChangeProjectBlock.cpp \
    block_implementations/Luminosus/DebugBlock.cpp \
    block_implementations/Luminosus/GroupBlock.cpp \
    block_implementations/Luminosus/GuiEditorBlock.cpp \
    block_implementations/Luminosus/LogBlock.cpp \
    block_implementations/Luminosus/LuminosusInfoBlock.cpp \
    block_implementations/Luminosus/PageAnchorBlock.cpp \
    block_implementations/MIDI/MidiControlInBlock.cpp \
    block_implementations/MIDI/MidiControlOutBlock.cpp \
    block_implementations/MIDI/MidiMonitorBlock.cpp \
    block_implementations/MIDI/MidiNoteInBlock.cpp \
    block_implementations/MIDI/MidiNoteInRangeBlock.cpp \
    block_implementations/MIDI/MidiNoteOutBlock.cpp \
    block_implementations/MIDI/MidiProgramInBlock.cpp \
    block_implementations/MIDI/MidiProgramOutBlock.cpp \
    block_implementations/OSC/OscInBlock.cpp \
    block_implementations/OSC/OscMonitorBlock.cpp \
    block_implementations/OSC/OscOutBlock.cpp \
    block_implementations/Theater/CueListBlock.cpp \
    block_implementations/Theater/CueListHelperBlock.cpp \
    block_implementations/Theater/DimmerBlock.cpp \
    block_implementations/Theater/MatrixBlock.cpp \
    block_implementations/Theater/PresetBlock.cpp \
    block_implementations/Theater/RgbLightBlock.cpp \
    block_implementations/Theater/RgbWAUVLightBlock.cpp \
    block_implementations/Theater/RgbWLightBlock.cpp \
    block_implementations/Theater/SceneSliderBlock.cpp \
    block_implementations/Theater/VerticalPatchBlock.cpp \
    block_implementations/X32/X32ChannelBlock.cpp \
    block_implementations/X32/X32OscMonitorBlock.cpp \
    block_implementations/X32/XAirAuxBlock.cpp \
    core/Cue.cpp \
    core/MainController.cpp \
    core/Matrix.cpp \
    core/NodeData.cpp \
    core/Nodes.cpp \
    core/SmartAttribute.cpp \
    core/block_data/BlockBase.cpp \
    core/block_data/BlockList.cpp \
    core/block_data/BlockList_blocks.cpp \
    core/block_data/FixtureBlock.cpp \
    core/block_data/InOutBlock.cpp \
    core/block_data/OneInputBlock.cpp \
    core/block_data/OneOutputBlock.cpp \
    core/manager/AnchorManager.cpp \
    core/manager/BlockManager.cpp \
    core/manager/Engine.cpp \
    core/manager/FileSystemManager.cpp \
    core/manager/GuiManager.cpp \
    core/manager/HandoffManager.cpp \
    core/manager/LogManager.cpp \
    core/manager/ProjectManager.cpp \
    core/manager/UpdateManager.cpp \
    eos_specific/EosActiveChannelsManager.cpp \
    eos_specific/EosCue.cpp \
    eos_specific/EosCueList.cpp \
    eos_specific/EosCueListManager.cpp \
    eos_specific/EosOSCManager.cpp \
    eos_specific/EosOSCMessage.cpp \
    light/ArtNetDiscoveryManager.cpp \
    light/ArtNetSender.cpp \
    light/OutputManager.cpp \
    midi/MidiManager.cpp \
    midi/MidiMappingManager.cpp \
    osc/GlobalOscCommands.cpp \
    osc/OSCMessage.cpp \
    osc/OSCNetworkManager.cpp \
    osc/OSCParser.cpp \
    other/PowermateListener.cpp \
    other/X32Manager.cpp \
    qtquick_items/AudioBarSpectrumItem.cpp \
    qtquick_items/AudioSpectrumItem.cpp \
    qtquick_items/BezierCurve.cpp \
    qtquick_items/CustomImagePainter.cpp \
    qtquick_items/FormulaBlockHighlighter.cpp \
    qtquick_items/KineticEffect.cpp \
    qtquick_items/KineticEffect2D.cpp \
    qtquick_items/LineItem.cpp \
    qtquick_items/NodeConnectionLines.cpp \
    qtquick_items/SpectralHistoryItem.cpp \
    qtquick_items/SpectrumItem.cpp \
    qtquick_items/StretchLayouts.cpp \
    qtquick_items/TouchArea.cpp \
    core/manager/KeyboardEmulator.cpp \
    block_implementations/Logic/KeyPressBlock.cpp \
    block_implementations/Logic/PresentationSlideBlock.cpp \
    block_implementations/Logic/PresentationRemoteBlock.cpp \
    block_implementations/Eos/EosSpeedMasterBlock.cpp \
    sacn/sacnlistener.cpp \
    sacn/sacnsender.cpp \
    sacn/sacnsocket.cpp \
    sacn/sacnuniverselistmodel.cpp \
    sacn/streamcommon.cpp \
    sacn/streamingacn.cpp \
    sacn/ACNShare/CID.cpp \
    sacn/ACNShare/ipaddr.cpp \
    sacn/ACNShare/tock.cpp \
    sacn/ACNShare/VHD.cpp \
    block_implementations/sACN/SAcnInBlock.cpp \
    block_implementations/Logic/DmxValueRangeBlock.cpp \
    block_implementations/Logic/PresentationSlideDmxBlock.cpp \
    light/BasicSAcnSender.cpp \
    block_implementations/sACN/VirtualFixtureBlock.cpp \
    block_implementations/Hog4/Hog4FaderBlock.cpp \
    eos_specific/HogOSCManager.cpp \
    eos_specific/OSCDiscovery.cpp

HEADERS += \
    audio/AudioEngine.h \
    audio/AudioInputAnalyzer.h \
    audio/AudioPlayerQt.h \
    audio/AudioWaveform.h \
    audio/QWaveDecoder.h \
    audio/SpeechInputAnalyzer.h \
    block_implementations/Audio/AudioLevelBlock.h \
    block_implementations/Audio/AudioPlaybackBlock.h \
    block_implementations/Audio/AudioStreamingBlock.h \
    block_implementations/Audio/AudioVolumeBlock.h \
    block_implementations/Audio/BeatDetectionBlock.h \
    block_implementations/Audio/BpmBlock.h \
    block_implementations/Controls/ButtonBlock.h \
    block_implementations/Controls/ColorwheelBlock.h \
    block_implementations/Controls/PixelControlBlock.h \
    block_implementations/Controls/SliderBlock.h \
    block_implementations/Controls/SwitchBlock.h \
    block_implementations/Eos/EosChanBlock.h \
    block_implementations/Eos/EosCmdBlock.h \
    block_implementations/Eos/EosCueBlock.h \
    block_implementations/Eos/EosCueListBlock.h \
    block_implementations/Eos/EosEffectBlock.h \
    block_implementations/Eos/EosEncoderBlock.h \
    block_implementations/Eos/EosFaderBankBlock.h \
    block_implementations/Eos/EosGroupBlock.h \
    block_implementations/Eos/EosInfoBlock.h \
    block_implementations/Eos/EosKeyBlock.h \
    block_implementations/Eos/EosMacroBlock.h \
    block_implementations/Eos/EosMasterPlaybackBlock.h \
    block_implementations/Eos/EosOscMonitorBlock.h \
    block_implementations/Eos/EosSingleFaderBlock.h \
    block_implementations/Eos/EosSubBlock.h \
    block_implementations/Eos/EosWheelsBlock.h \
    block_implementations/Info/ClockBlock.h \
    block_implementations/Info/ImageBlock.h \
    block_implementations/Info/NotesBlock.h \
    block_implementations/Info/PreviewBlock.h \
    block_implementations/Info/QmlOnlyBlock.h \
    block_implementations/Info/StopwatchBlock.h \
    block_implementations/Info/StyledTextBlock.h \
    block_implementations/Info/VideoBlock.h \
    block_implementations/Logic/BasicTransformBlock.h \
    block_implementations/Logic/ColorizeBlock.h \
    block_implementations/Logic/CommandlineBlock.h \
    block_implementations/Logic/CrossfadeBlock.h \
    block_implementations/Logic/DecayBlock.h \
    block_implementations/Logic/DelayBlock.h \
    block_implementations/Logic/HoldMaxBlock.h \
    block_implementations/Logic/HsvBlock.h \
    block_implementations/Logic/LinearValueBlock.h \
    block_implementations/Logic/MovingPatternBlock.h \
    block_implementations/Logic/MultiplyBlock.h \
    block_implementations/Logic/OffsetBlock.h \
    block_implementations/Logic/OneDimensionalPattern.h \
    block_implementations/Logic/ProjectorShutterBlock.h \
    block_implementations/Logic/RandomValueBlock.h \
    block_implementations/Logic/RecorderBlock.h \
    block_implementations/Logic/RecorderSlaveBlock.h \
    block_implementations/Logic/ScriptBlock.h \
    block_implementations/Logic/SequencerBlock.h \
    block_implementations/Logic/SinusValueBlock.h \
    block_implementations/Logic/SmoothBlock.h \
    block_implementations/Logic/ThresholdBlock.h \
    block_implementations/Logic/TickGeneratorBlock.h \
    block_implementations/Logic/TimerBlock.h \
    block_implementations/Logic/ToggleBlock.h \
    block_implementations/Logic/ValueProjectionBlock.h \
    block_implementations/Luminosus/ChangeProjectBlock.h \
    block_implementations/Luminosus/ControlsTestBlock.h \
    block_implementations/Luminosus/DebugBlock.h \
    block_implementations/Luminosus/GroupBlock.h \
    block_implementations/Luminosus/GuiEditorBlock.h \
    block_implementations/Luminosus/LogBlock.h \
    block_implementations/Luminosus/LuminosusInfoBlock.h \
    block_implementations/Luminosus/PageAnchorBlock.h \
    block_implementations/MIDI/MidiControlInBlock.h \
    block_implementations/MIDI/MidiControlOutBlock.h \
    block_implementations/MIDI/MidiMonitorBlock.h \
    block_implementations/MIDI/MidiNoteInBlock.h \
    block_implementations/MIDI/MidiNoteInRangeBlock.h \
    block_implementations/MIDI/MidiNoteOutBlock.h \
    block_implementations/MIDI/MidiProgramInBlock.h \
    block_implementations/MIDI/MidiProgramOutBlock.h \
    block_implementations/OSC/OscInBlock.h \
    block_implementations/OSC/OscMonitorBlock.h \
    block_implementations/OSC/OscOutBlock.h \
    block_implementations/Theater/CueListBlock.h \
    block_implementations/Theater/CueListHelperBlock.h \
    block_implementations/Theater/DimmerBlock.h \
    block_implementations/Theater/MatrixBlock.h \
    block_implementations/Theater/PresetBlock.h \
    block_implementations/Theater/RgbLightBlock.h \
    block_implementations/Theater/RgbWAUVLightBlock.h \
    block_implementations/Theater/RgbWLightBlock.h \
    block_implementations/Theater/SceneSliderBlock.h \
    block_implementations/Theater/VerticalPatchBlock.h \
    block_implementations/X32/X32ChannelBlock.h \
    block_implementations/X32/X32OscMonitorBlock.h \
    block_implementations/X32/XAirAuxBlock.h \
    core/Cue.h \
    core/MainController.h \
    core/Matrix.h \
    core/NodeData.h \
    core/Nodes.h \
    core/QCircularBuffer.h \
    core/SmartAttribute.h \
    core/block_data/BlockBase.h \
    core/block_data/BlockInterface.h \
    core/block_data/BlockList.h \
    core/block_data/FixtureBlock.h \
    core/block_data/InOutBlock.h \
    core/block_data/OneInputBlock.h \
    core/block_data/OneOutputBlock.h \
    core/block_data/SceneBlockInterface.h \
    core/manager/AnchorManager.h \
    core/manager/BlockManager.h \
    core/manager/Engine.h \
    core/manager/FileSystemManager.h \
    core/manager/GuiManager.h \
    core/manager/HandoffManager.h \
    core/manager/LogManager.h \
    core/manager/ProjectManager.h \
    core/manager/UpdateManager.h \
    eos_specific/EosActiveChannelsManager.h \
    eos_specific/EosCue.h \
    eos_specific/EosCueList.h \
    eos_specific/EosCueListManager.h \
    eos_specific/EosOSCManager.h \
    eos_specific/EosOSCMessage.h \
    ffft/Array.h \
    ffft/Array.hpp \
    ffft/DynArray.h \
    ffft/DynArray.hpp \
    ffft/FFTReal.h \
    ffft/FFTReal.hpp \
    ffft/FFTRealFixLen.h \
    ffft/FFTRealFixLen.hpp \
    ffft/FFTRealFixLenParam.h \
    ffft/FFTRealPassDirect.h \
    ffft/FFTRealPassDirect.hpp \
    ffft/FFTRealPassInverse.h \
    ffft/FFTRealPassInverse.hpp \
    ffft/FFTRealSelect.h \
    ffft/FFTRealSelect.hpp \
    ffft/FFTRealUseTrigo.h \
    ffft/FFTRealUseTrigo.hpp \
    ffft/OscSinCos.h \
    ffft/OscSinCos.hpp \
    ffft/def.h \
    light/ArtNetDiscoveryManager.h \
    light/ArtNetSender.h \
    light/OutputManager.h \
    midi/MidiManager.h \
    midi/MidiMappingManager.h \
    osc/GlobalOscCommands.h \
    osc/OSCMessage.h \
    osc/OSCNetworkManager.h \
    osc/OSCParser.h \
    other/PowermateListener.h \
    other/X32Manager.h \
    qtquick_items/AudioBarSpectrumItem.h \
    qtquick_items/AudioSpectrumItem.h \
    qtquick_items/BezierCurve.h \
    qtquick_items/CustomImagePainter.h \
    qtquick_items/FormulaBlockHighlighter.h \
    qtquick_items/KineticEffect.h \
    qtquick_items/KineticEffect2D.h \
    qtquick_items/LineItem.h \
    qtquick_items/NodeConnectionLines.h \
    qtquick_items/SpectralHistoryItem.h \
    qtquick_items/SpectrumItem.h \
    qtquick_items/StretchLayouts.h \
    qtquick_items/TouchArea.h \
    utils.h \
    version.h \
    core/manager/KeyboardEmulator.h \
    block_implementations/Logic/KeyPressBlock.h \
    block_implementations/Logic/PresentationSlideBlock.h \
    block_implementations/Logic/PresentationRemoteBlock.h \
    block_implementations/Eos/EosSpeedMasterBlock.h \
    sacn/sacnlistener.h \
    sacn/sacnsender.h \
    sacn/sacnsocket.h \
    sacn/sacnuniverselistmodel.h \
    sacn/streamcommon.h \
    sacn/streamingacn.h \
    sacn/ACNShare/CID.h \
    sacn/ACNShare/defpack.h \
    sacn/ACNShare/deftypes.h \
    sacn/ACNShare/ipaddr.h \
    sacn/ACNShare/tock.h \
    sacn/ACNShare/VHD.h \
    sacn/consts.h \
    block_implementations/sACN/SAcnInBlock.h \
    block_implementations/Logic/DmxValueRangeBlock.h \
    block_implementations/Logic/PresentationSlideDmxBlock.h \
    light/BasicSAcnSender.h \
    block_implementations/sACN/VirtualFixtureBlock.h \
    block_implementations/Hog4/Hog4FaderBlock.h \
    eos_specific/HogOSCManager.h \
    eos_specific/OSCDiscovery.h

DISTFILES += \
    CREDITS.txt \
    doc/Changelog.txt \
    LICENSE.txt \
    README.md \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat \
    luminosus.rc \
    Doxyfile \
    ios/Info.plist \
    ios/artwork/Icon_60@2x.png \
    ios/artwork/Icon_60@3x.png \
    ios/artwork/Icon_76.png \
    ios/artwork/Icon_76@2x.png \
    ios/artwork/iTunesArtwork.png \
    ios/artwork/iTunesArtwork@2x.png \
    amazon_dash.py \
    GoogleCloudSpeech_gRPC_Notes.txt \
    speech_recognition_server.py

# ------------------ OS specific -----------------------

# set OS specific configuration flags:
unix:!macx:!android:!ios {
    CONFIG += linux_specific
}
ios {
    CONFIG += mobile_platform

    QMAKE_INFO_PLIST = ios/Info.plist
    ios_icon.files = $$files($$PWD/ios/artwork/Icon_*.png)
    QMAKE_BUNDLE_DATA += ios_icon
    app_launch_images.files = $$PWD/ios/CustomLaunchScreen.xib
    app_launch_images.files += $$files($$PWD/ios/artwork/iTunes*.png)
    QMAKE_BUNDLE_DATA += app_launch_images
    QMAKE_IOS_DEPLOYMENT_TARGET = 9.0
}
android {
    QT += androidextras
    CONFIG += mobile_platform
}

# include alsa and pthread on Linux for RtMidi
linux_specific {
    DEFINES += __LINUX_ALSA__
    LIBS += -L/alsa/ -lasound -lpthread
}

# Temporary add VLC-Qt lib for linux till a solution for the GStreamer bug with Ubuntu is found:
linux_specific {
    DEFINES += USE_VLC_LIB
    LIBS += -lVLCQtCore
    SOURCES += audio/AudioPlayerVlc.cpp
    HEADERS += audio/AudioPlayerVlc.h
}

# include winmm lib on Windows for RtMidi
win32:LIBS += -lwinmm
win64:LIBS += -lwinmm

#include CoreMIDI framework on Mac for RtMidi
macx:LIBS += -framework CoreMIDI -framework CoreAudio -framework CoreFoundation

# Build with RtMidi for Midi support on Windows, Linux and Mac, but not on mobile:
!mobile_platform {
    DEFINES += RT_MIDI_AVAILABLE
    SOURCES += RtMidi/RtMidi.cpp
    HEADERS += RtMidi/RtMidi.h
}

# Android specific files:
ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
