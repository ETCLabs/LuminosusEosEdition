
#include "KeyboardEmulator.h"

#include <QThread>
#include <QDebug>

KeyboardEmulator::KeyboardEmulator(QObject* parent)
    : QObject (parent)
{

}

#ifdef Q_OS_WIN

#define WINVER 0x0500
#include <windows.h>

INPUT createInput(quint32 scanCode, bool isPressed, bool extendedKey) {
    INPUT input;
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = 0;
    input.ki.dwFlags = KEYEVENTF_SCANCODE;
    if (!isPressed) input.ki.dwFlags |= KEYEVENTF_KEYUP;
    if (extendedKey) input.ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;
    input.ki.wScan = static_cast<WORD>(scanCode);
    input.ki.time = 0;
    input.ki.dwExtraInfo = static_cast<unsigned long>(GetMessageExtraInfo());
    return input;
}

void KeyboardEmulator::pressKey(quint32 scanCode, bool extendedKey) {
    INPUT input = createInput(scanCode, true, extendedKey);
    SendInput(1, &input, sizeof(INPUT));
}

void KeyboardEmulator::releaseKey(quint32 scanCode, bool extendedKey) {
    INPUT input = createInput(scanCode, false, extendedKey);
    SendInput(1, &input, sizeof(INPUT));
}

#else

void KeyboardEmulator::pressKey(quint32 scanCode, bool /*extendedKey*/) {
    qDebug() << "[KEY_EMU] pressing  " << scanCode;
}

void KeyboardEmulator::releaseKey(quint32 scanCode, bool /*extendedKey*/) {
    qDebug() << "[KEY_EMU] releasing " << scanCode;
}

#endif
