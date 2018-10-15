#ifndef KEYBOARDEMULATOR_H
#define KEYBOARDEMULATOR_H

#include <QObject>

namespace ScanCode {

static const quint32 LEFT = 75;
static const quint32 RIGHT = 77;
static const quint32 RETURN = 28;
static const quint32 W = 17;
static const quint32 B = 48;

static const quint32 NUMBER_0 = 11;
static const quint32 NUMBER_1 = 2;
static const quint32 NUMBER_2 = 3;
static const quint32 NUMBER_3 = 4;
static const quint32 NUMBER_4 = 5;
static const quint32 NUMBER_5 = 6;
static const quint32 NUMBER_6 = 7;
static const quint32 NUMBER_7 = 8;
static const quint32 NUMBER_8 = 9;
static const quint32 NUMBER_9 = 10;

}


class KeyboardEmulator : public QObject {

    Q_OBJECT

public:
    explicit KeyboardEmulator(QObject* parent);

    void pressKey(quint32 scanCode, bool extendedKey = false);
    void releaseKey(quint32 scanCode, bool extendedKey = false);
};

#endif // KEYBOARDEMULATOR_H
