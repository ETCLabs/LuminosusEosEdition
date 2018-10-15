#include "StopwatchBlock.h"

#include "core/MainController.h"
#include "core/Nodes.h"


StopwatchBlock::StopwatchBlock(MainController *controller, QString uid)
    : BlockBase(controller, uid)
    , m_startInput(nullptr)
    , m_stopInput(nullptr)
    , m_resetInput(nullptr)
    , m_lastStartValue(0.0)
    , m_lastStopValue(0.0)
    , m_lastResetValue(0.0)
    , m_lastSessionTime(0.0)
    , m_beginCurrentSession(HighResTime::now())
    , m_running(false)
{
    // prepare nodes:
    m_startInput = createInputNode("startInput");
    m_stopInput = createInputNode("stopInput");
    m_resetInput = createInputNode("resetInput");

    //connect signals and slots:
    connect(m_startInput, SIGNAL(dataChanged()), this, SLOT(onStartInputChanged()));
    connect(m_stopInput, SIGNAL(dataChanged()), this, SLOT(onStopInputChanged()));
    connect(m_resetInput, SIGNAL(dataChanged()), this, SLOT(onResetInputChanged()));
}

void StopwatchBlock::getAdditionalState(QJsonObject& state) const {
    state["lastSessionTime"] = m_lastSessionTime;
}

void StopwatchBlock::setAdditionalState(const QJsonObject &state) {
    m_lastSessionTime = state["lastSessionTime"].toDouble();
    emit timeStringChanged();
}

void StopwatchBlock::onStartInputChanged() {
    double value = m_startInput->getValue();
    if (value >= LuminosusConstants::triggerThreshold
            && m_lastStartValue < LuminosusConstants::triggerThreshold) {
        start();
    }
    m_lastStartValue = value;
}

void StopwatchBlock::onStopInputChanged() {
    double value = m_stopInput->getValue();
    if (value >= LuminosusConstants::triggerThreshold
            && m_lastStopValue < LuminosusConstants::triggerThreshold) {
        stop();
    }
    m_lastStopValue = value;
}

void StopwatchBlock::onResetInputChanged() {
    double value = m_resetInput->getValue();
    if (value >= LuminosusConstants::triggerThreshold
            && m_lastResetValue < LuminosusConstants::triggerThreshold) {
        reset();
    }
    m_lastResetValue = value;
}

void StopwatchBlock::start() {
    if (m_running) return;
    setRunning(true);
    m_beginCurrentSession = HighResTime::now();
    emit timeStringChanged();
}

void StopwatchBlock::stop() {
    if (!m_running) return;
    setRunning(false);
    m_lastSessionTime += HighResTime::elapsedSecSince(m_beginCurrentSession);
    emit timeStringChanged();
}

void StopwatchBlock::reset() {
    if (m_running) stop();
    m_lastSessionTime = 0.0;
    emit timeStringChanged();
}

QString StopwatchBlock::getTimeString() const {
    double elapsedSec = m_lastSessionTime;
    if (m_running) {
        elapsedSec += HighResTime::elapsedSecSince(m_beginCurrentSession);
    }
    int dsec = fmod(elapsedSec, 1) * 10;
    int secs = fmod(elapsedSec, 60);
    int mins = fmod((elapsedSec / 60), 60);
    int hours = fmod((elapsedSec / 3600), 60);
    QString timeString = QString("%1:%2:%3.%4").arg(hours, 2, 10, QChar('0')).arg(mins, 2, 10, QChar('0')).arg(secs, 2, 10, QChar('0')).arg(dsec);
    return timeString;
}
