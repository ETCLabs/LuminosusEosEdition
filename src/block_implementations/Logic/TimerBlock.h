#ifndef TIMERBLOCK_H
#define TIMERBLOCK_H

#include "core/block_data/InOutBlock.h"
#include "core/SmartAttribute.h"

#include <QTimer>


class TimerBlock : public InOutBlock
{
    Q_OBJECT

public:

    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Timer";
        info.nameInUi = "Timer";
        info.category << "General" << "Logic";
        info.helpText = "";
        info.visibilityRequirements << VisibilityRequirement::DeveloperMode;
        info.qmlFile = "qrc:/qml/Blocks/Logic/TimerBlock.qml";
        info.complete<TimerBlock>();
        return info;
    }

    explicit TimerBlock(MainController* controller, QString uid);

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    QString getRemainingTimeString() const;

private slots:
    void start();
    void onTimerEnd();
    void onImpulseEnd();

protected:
    QTimer m_timer;
    BoolAttribute m_running;
};

#endif // TIMERBLOCK_H
