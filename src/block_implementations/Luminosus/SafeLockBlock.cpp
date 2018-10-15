#include "SafeLockBlock.h"

#include "core/MainController.h"
#include "core/Nodes.h"
#include "utils.h"
#include "conversation/DecisionEngine.h"

#include <QTimer>


SafeLockBlock::SafeLockBlock(MainController* controller, QString uid)
    : InOutBlock(controller, uid)
    , m_angle(this, "angle", 0, 0, 360, /*persistent*/ false)
    , m_code1(this, "code1", 20, 0, 99)
    , m_code2(this, "code2", 60, 0, 99)
    , m_code3(this, "code3", 40, 0, 99)
    , m_try1(this, "try1", -1, -1, 99, /*persistent*/ false)
    , m_try2(this, "try2", -1, -1, 99, /*persistent*/ false)
    , m_try3(this, "try3", -1, -1, 99, /*persistent*/ false)
    , m_valid(this, "valid", false, /*persistent*/ false)
    , m_direction(0.0)
    , m_otherDirection(0.0)
    , m_displayedItem(nullptr)
{
    m_inputNode->enableImpulseDetection();
    connect(m_inputNode, SIGNAL(impulseBegin()), this, SLOT(show()));
}

void SafeLockBlock::show() {
    if (m_displayedItem) hide();
    m_displayedItem = createQmlItem("qrc:/qml/Conversation/SafeLock.qml", m_controller->decisionEngine()->getOledGuiItem());
    focus();
    QTimer::singleShot(30*1000, this, SLOT(hide()));
}

void SafeLockBlock::hide() {
    if (!m_displayedItem) return;
    m_displayedItem->setParentItem(nullptr);
    m_displayedItem->deleteLater();
    m_displayedItem = nullptr;
    clear();
}

void SafeLockBlock::clear() {
    m_try1 = -1;
    m_try2 = -1;
    m_try3 = -1;
    m_direction = 0.0;
    m_otherDirection = 0.0;
    m_valid = false;
}

void SafeLockBlock::evaluate() {
    int error = 0;
    error += qAbs(m_code1 - m_try1);
    error += qAbs(m_code2 - m_try2);
    error += qAbs(m_code3 - m_try3);
    m_valid = error < 7;
    if (m_valid) {
        QTimer::singleShot(800, this, SLOT(hide()));
        m_outputNode->sendImpulse();
    }
}

void SafeLockBlock::onControllerRotated(double, double degrees, bool) {
    m_angle = realMod(m_angle + degrees, 360.0);
    if (qAbs(m_direction) < 0.1) {
        // -> first movement after clear
        m_direction = degrees;
        return;
    }
    if (m_direction * degrees < 0 && qAbs(m_direction) > 0.1) {
        // direction change, maybe unintended
        m_otherDirection += degrees;
        if (qAbs(m_otherDirection) > 8) {
            m_controller->playClickUpSound();
            // -> real direction change:
            double changedAtAngle = realMod(m_angle - m_otherDirection, 360.0);
            int number = qRound(realMod(100.0 - changedAtAngle / 3.6, 100.0));
            m_try1 = m_try2.getValue();
            m_try2 = m_try3.getValue();
            m_try3 = number;
            evaluate();
            m_direction = degrees;
            m_otherDirection = 0.0;
        }
    } else {
        // continuing in same direction:
        m_otherDirection = 0.0;
    }
}
