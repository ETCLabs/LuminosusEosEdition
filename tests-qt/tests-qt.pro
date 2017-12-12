
# ------------------ General configurations -----------------------

TEMPLATE = app

QT += qml quick multimedia svg testlib

CONFIG += c++11

DEFINES += QT_MESSAGELOGCONTEXT

# Google Test
LIBS += -L/usr/local/lib -lgtest -lgmock

# ------------------ Resources (.cpp, .h, .qrc, icons) -----------------------

SOURCES += \
    main.cpp \
    ../src/Matrix.cpp

HEADERS += \
    ../src/Matrix.h \
    MatrixTest.h

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

# include winmm lib on Windows for RtMidi
win32:LIBS += -lwinmm
win64:LIBS += -lwinmm

#include CoreMIDI framework on Mac for RtMidi
macx:LIBS += -framework CoreMIDI -framework CoreAudio -framework CoreFoundation

# Build with RtMidi for Midi support on Windows, Linux and Mac, but not on mobile:
#!mobile_platform {
#    DEFINES += RT_MIDI_AVAILABLE
#    SOURCES += RtMidi/RtMidi.cpp
#    HEADERS += RtMidi/RtMidi.h
#}
