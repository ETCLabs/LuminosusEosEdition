#ifndef STOPWATCHBLOCK_H
#define STOPWATCHBLOCK_H

#include "core/block_data/BlockBase.h"
#include "utils.h"


class StopwatchBlock : public BlockBase {

    Q_OBJECT

    Q_PROPERTY(bool running MEMBER m_running NOTIFY runningChanged)

public:

    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Stopwatch";
        info.category << "General" << "Other";
        info.helpText = "A simple stopwatch.\n\n"
                        "Use it for example by connecting Eos Cue Blocks to the start "
                        "and stop inputs to measure the time between two Cues.";
        info.qmlFile = "qrc:/qml/Blocks/Info/StopwatchBlock.qml";
        info.complete<StopwatchBlock>();
        return info;
    }

    StopwatchBlock(MainController* controller, QString uid);

    virtual void getAdditionalState(QJsonObject& state) const override;
    virtual void setAdditionalState(const QJsonObject& state) override;

signals:
    void timeStringChanged();
    void runningChanged();

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    void onStartInputChanged();
    void onStopInputChanged();
    void onResetInputChanged();

    void start();
    void stop();
    void reset();

    QString getTimeString() const;

    void setRunning(bool value) { m_running = value; emit runningChanged(); }

protected:
    QPointer<NodeBase> m_startInput;
    QPointer<NodeBase> m_stopInput;
    QPointer<NodeBase> m_resetInput;

    double m_lastStartValue;
    double m_lastStopValue;
    double m_lastResetValue;

    double m_lastSessionTime;
    HighResTime::time_point_t m_beginCurrentSession;

    bool m_running;
};

#endif // STOPWATCHBLOCK_H
