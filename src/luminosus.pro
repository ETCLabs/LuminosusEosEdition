
# ------------------ General configurations -----------------------

TEMPLATE = app

QT += qml quick multimedia svg

CONFIG += c++11

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

DEFINES += QT_MESSAGELOGCONTEXT

# use QtQuick Compiler (only available under commercial license)
CONFIG += qtquickcompiler


# ------------------ Resources (.cpp, .h, .qrc, icons) -----------------------

SOURCES += main.cpp \
    ArtNetSender.cpp \
    AudioEngine.cpp \
    BezierCurve.cpp \
    Engine.cpp \
    MainController.cpp \
    Matrix.cpp \
    MidiManager.cpp \
    OutputManager.cpp \
    block_data/OneOutputBlock.cpp \
    FileSystemManager.cpp \
    PowermateListener.cpp \
    block_data/InOutBlock.cpp \
    block_data/OneInputBlock.cpp \
    KineticEffect.cpp \
    OSCMessage.cpp \
    OSCNetworkManager.cpp \
    OSCParser.cpp \
    block_implementations/AudioLevelBlock.cpp \
    block_implementations/ColorwheelBlock.cpp \
    block_implementations/DimmerBlock.cpp \
    block_implementations/ScriptBlock.cpp \
    block_implementations/SliderBlock.cpp \
    block_implementations/SwitchBlock.cpp \
    block_data/BlockBase.cpp \
    block_data/BlockList.cpp \
    block_data/BlockManager.cpp \
    block_implementations/EosChanBlock.cpp \
    block_implementations/OscOutBlock.cpp \
    block_implementations/OscInBlock.cpp \
    block_implementations/DelayBlock.cpp \
    block_implementations/ThresholdBlock.cpp \
    block_implementations/DecayBlock.cpp \
    block_implementations/OscMonitorBlock.cpp \
    KineticEffect2D.cpp \
    block_implementations/MidiNoteInBlock.cpp \
    block_implementations/MidiNoteInRangeBlock.cpp \
    block_implementations/MidiNoteOutBlock.cpp \
    block_implementations/MidiControlOutBlock.cpp \
    block_implementations/MidiControlInBlock.cpp \
    block_implementations/MidiMonitorBlock.cpp \
    block_implementations/MidiProgramInBlock.cpp \
    block_implementations/MidiProgramOutBlock.cpp \
    block_implementations/EosCueBlock.cpp \
    block_implementations/BpmBlock.cpp \
    block_implementations/HoldMaxBlock.cpp \
    block_implementations/RandomValueBlock.cpp \
    GlobalOscCommands.cpp \
    StretchLayouts.cpp \
    ProjectManager.cpp \
    block_implementations/ButtonBlock.cpp \
    block_implementations/EosKeyBlock.cpp \
    block_implementations/DebugBlock.cpp \
    block_implementations/NotesBlock.cpp \
    block_implementations/PreviewBlock.cpp \
    LogManager.cpp \
    block_implementations/LogBlock.cpp \
    block_implementations/EosCmdBlock.cpp \
    block_implementations/EosInfoBlock.cpp \
    eos_specific/EosOSCManager.cpp \
    eos_specific/EosOSCMessage.cpp \
    eos_specific/EosCue.cpp \
    eos_specific/EosCueList.cpp \
    eos_specific/EosCueListManager.cpp \
    block_implementations/EosCueListBlock.cpp \
    TouchArea.cpp \
    block_implementations/EosFaderBankBlock.cpp \
    block_implementations/EosMasterPlaybackBlock.cpp \
    block_implementations/LuminosusInfoBlock.cpp \
    eos_specific/EosActiveChannelsManager.cpp \
    block_implementations/EosWheelsBlock.cpp \
    block_implementations/EosOscMonitorBlock.cpp \
    block_implementations/EosSubBlock.cpp \
    block_implementations/EosGroupBlock.cpp \
    block_implementations/EosSingleFaderBlock.cpp \
    Nodes.cpp \
    NodeConnectionLines.cpp \
    block_implementations/EosMacroBlock.cpp \
    block_implementations/StopwatchBlock.cpp \
    block_implementations/QmlOnlyBlock.cpp \
    block_implementations/AudioPlaybackBlock.cpp \
    block_implementations/ClockBlock.cpp \
    block_implementations/LinearValueBlock.cpp \
    block_implementations/SinusValueBlock.cpp \
    block_implementations/TickGeneratorBlock.cpp \
    UpdateManager.cpp \
    SpectrumItem.cpp \
    AudioInputAnalyzer.cpp \
    block_implementations/BeatDetectionBlock.cpp \
    SpectralHistoryItem.cpp \
    AudioSpectrumItem.cpp \
    block_implementations/AudioVolumeBlock.cpp \
    block_implementations/EosEffectBlock.cpp \
    AudioPlayerQt.cpp \
    AnchorManager.cpp \
    block_implementations/PageAnchorBlock.cpp \
    block_implementations/StyledTextBlock.cpp \
    block_implementations/ImageBlock.cpp


RESOURCES += qml.qrc \
    images.qrc \
    fonts.qrc \
    qmlBlocks.qrc \
    examples.qrc

HEADERS += \
    ffft/Array.h \
    ffft/Array.hpp \
    ffft/def.h \
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
    utils.h \
    MainController.h \
    Engine.h \
    BezierCurve.h \
    AudioEngine.h \
    ArtNetSender.h \
    Matrix.h \
    MidiManager.h \
    OutputManager.h \
    PowermateListener.h \
    block_data/OneOutputBlock.h \
    FileSystemManager.h \
    block_data/InOutBlock.h \
    block_data/OneInputBlock.h \
    KineticEffect.h \
    OSCMessage.h \
    OSCNetworkManager.h \
    OSCParser.h \
    QCircularBuffer.h \
    block_implementations/AudioLevelBlock.h \
    block_implementations/ButtonBlock.h \
    block_implementations/ColorwheelBlock.h \
    block_implementations/ControlsTestBlock.h \
    block_implementations/DimmerBlock.h \
    block_implementations/ScriptBlock.h \
    block_implementations/SliderBlock.h \
    block_implementations/SwitchBlock.h \
    block_data/BlockBase.h \
    block_data/BlockInterface.h \
    block_data/BlockList.h \
    block_data/BlockManager.h \
    block_implementations/EosChanBlock.h \
    block_implementations/OscOutBlock.h \
    block_implementations/OscInBlock.h \
    block_implementations/DelayBlock.h \
    block_implementations/ThresholdBlock.h \
    block_implementations/DecayBlock.h \
    block_implementations/OscMonitorBlock.h \
    KineticEffect2D.h \
    block_implementations/MidiNoteInBlock.h \
    block_implementations/MidiNoteInRangeBlock.h \
    block_implementations/MidiNoteOutBlock.h \
    block_implementations/MidiControlOutBlock.h \
    block_implementations/MidiControlInBlock.h \
    block_implementations/MidiMonitorBlock.h \
    block_implementations/MidiProgramInBlock.h \
    block_implementations/MidiProgramOutBlock.h \
    block_implementations/EosCueBlock.h \
    block_implementations/BpmBlock.h \
    block_implementations/HoldMaxBlock.h \
    block_implementations/RandomValueBlock.h \
    GlobalOscCommands.h \
    StretchLayouts.h \
    ProjectManager.h \
    block_implementations/EosKeyBlock.h \
    version.h \
    block_implementations/DebugBlock.h \
    block_implementations/NotesBlock.h \
    block_implementations/PreviewBlock.h \
    LogManager.h \
    block_implementations/LogBlock.h \
    block_implementations/EosCmdBlock.h \
    block_implementations/EosInfoBlock.h \
    eos_specific/EosOSCManager.h \
    eos_specific/EosOSCMessage.h \
    eos_specific/EosCue.h \
    eos_specific/EosCueList.h \
    eos_specific/EosCueListManager.h \
    block_implementations/EosCueListBlock.h \
    TouchArea.h \
    block_implementations/EosFaderBankBlock.h \
    block_implementations/EosMasterPlaybackBlock.h \
    block_implementations/LuminosusInfoBlock.h \
    eos_specific/EosActiveChannelsManager.h \
    block_implementations/EosWheelsBlock.h \
    block_implementations/EosOscMonitorBlock.h \
    block_implementations/EosSubBlock.h \
    block_implementations/EosGroupBlock.h \
    block_implementations/EosSingleFaderBlock.h \
    Nodes.h \
    NodeConnectionLines.h \
    block_implementations/EosMacroBlock.h \
    block_implementations/StopwatchBlock.h \
    block_implementations/QmlOnlyBlock.h \
    block_implementations/AudioPlaybackBlock.h \
    block_implementations/ClockBlock.h \
    block_implementations/LinearValueBlock.h \
    block_implementations/SinusValueBlock.h \
    block_implementations/TickGeneratorBlock.h \
    UpdateManager.h \
    SpectrumItem.h \
    AudioInputAnalyzer.h \
    block_implementations/BeatDetectionBlock.h \
    SpectralHistoryItem.h \
    AudioSpectrumItem.h \
    block_implementations/AudioVolumeBlock.h \
    block_implementations/EosEffectBlock.h \
    AudioPlayerQt.h \
    #AudioPlayerVlc.h
    AnchorManager.h \
    block_implementations/PageAnchorBlock.h \
    block_implementations/StyledTextBlock.h \
    block_implementations/ImageBlock.h

ICON = images/icon/app_icon_icns.icns

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
    Doxyfile

# Windows .exe icon:
RC_FILE = luminosus.rc

# ------------------ OS specific -----------------------

# set OS specific configuration flags:
unix:!macx:!android:!ios {
    CONFIG += linux_specific
}
ios {
    CONFIG += mobile_platform
}
android {
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
    SOURCES += AudioPlayerVlc.cpp
    HEADERS += AudioPlayerVlc.h
}

#DEFINES += USE_VLC_LIB
#LIBS += -lVLCQtCore

#LIBS       += -L/Users/Tim/Documents/luminosus_qt/vlc-qt/lib -lVLCQtCore
#INCLUDEPATH += /Users/Tim/Documents/luminosus_qt/vlc-qt/vlc-qt/include

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

# ------------------ QmlTricks Submodules -----------------------

#include ($$PWD/libQtQmlTricks/SuperMacros/QtSuperMacros.pri)
#include ($$PWD/libQtQmlTricks/SmartDataModels/QtQmlModels.pri)
#include ($$PWD/libQtQmlTricks/UiElements/QtQuickUiElements.pri)
