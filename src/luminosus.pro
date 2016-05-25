
# ------------------ General configurations -----------------------

TEMPLATE = app

QT += qml quick multimedia

CONFIG += c++11

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)


# ------------------ Resources (.cpp, .h, .qrc, icons) -----------------------

SOURCES += main.cpp \
    ArtNetSender.cpp \
    AudioEngine.cpp \
    BezierCurve.cpp \
    Engine.cpp \
    MainController.cpp \
    Matrix.cpp \
    MidiManager.cpp \
    NodeBase.cpp \
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
    SvgIconHelper.cpp \
    block_implementations/NotesBlock.cpp


RESOURCES += qml.qrc \
    images.qrc \
    fonts.qrc \
    qmlBlocks.qrc

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
    NodeBase.h \
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
    SvgIconHelper.h \
    block_implementations/NotesBlock.h

ICON = images/icon/app_icon_icns.icns

DISTFILES += \
    Doxyfile \
    CREDITS.txt \
    doc/Changelog.txt \
    LICENSE.txt \
    README.txt

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

# include winmm lib on Windowsfor RtMidi
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

# ------------------ QmlTricks Submodules -----------------------

include ($$PWD/libQtQmlTricks/SuperMacros/QtSuperMacros.pri)
include ($$PWD/libQtQmlTricks/SmartDataModels/QtQmlModels.pri)
include ($$PWD/libQtQmlTricks/UiElements/QtQuickUiElements.pri)
