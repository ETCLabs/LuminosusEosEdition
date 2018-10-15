#ifndef POWERMATELISTENER_H
#define POWERMATELISTENER_H

#include <QFile>
#include <QThread>

#if defined(Q_OS_LINUX)
    #include <linux/input.h>
#endif


class PowermateListener : public QThread {

    Q_OBJECT

    Q_PROPERTY(bool connected READ connected NOTIFY connectedChanged)

public:
    explicit PowermateListener(QObject *parent = 0);

#if defined(Q_OS_LINUX)
    void processEvent(const input_event& event);
#endif

signals:
    void rotated(double relativeAmount, double degrees, bool pressed);
    void pressed();
    void released(double duration);

    void connectedChanged();

public slots:
    void setBrightness(double);
    void startPulsing();

    bool connected() const;

protected:
    void run();

private:
    bool m_isPressed;
    double m_lastPressTime;
    double m_lastRotationTime;
    QFile m_outputDevice;

};

#endif // POWERMATELISTENER_H
