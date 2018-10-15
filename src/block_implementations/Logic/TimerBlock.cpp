#include "TimerBlock.h"

#include "core/MainController.h"
#include "core/Nodes.h"


TimerBlock::TimerBlock(MainController* controller, QString uid)
    : InOutBlock(controller, uid)
    , m_running(this, "running", false, /*persistent*/ false)
{
    m_timer.setSingleShot(true);
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(onTimerEnd()));
    m_inputNode->enableImpulseDetection();
    connect(m_inputNode, SIGNAL(impulseBegin()), this, SLOT(start()));
}

QString TimerBlock::getRemainingTimeString() const {
    int msecs = m_timer.remainingTime();
    int secs = (msecs / 1000) % 60;
    int minutes = msecs / 60000;
    QString str = QString("%1:%2").arg(minutes, 2, 10,  QLatin1Char('0')).arg(secs, 2, 10,  QLatin1Char('0'));
    return str;
}

void TimerBlock::start() {
    if (!m_inputNode->constData().absoluteMaximumIsProvided()) {
        m_timer.stop();
        m_running = false;
        return;
    }

    double secs = m_inputNode->constData().getAbsoluteMaximum();
    m_timer.start(secs * 1000);
    m_running = true;
}

void TimerBlock::onTimerEnd() {
    m_outputNode->setValue(1.0);
    QTimer::singleShot(200, this, SLOT(onImpulseEnd()));
    m_running = false;
}

void TimerBlock::onImpulseEnd() {
    m_outputNode->setValue(0.0);
}
