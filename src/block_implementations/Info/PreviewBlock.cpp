#include "PreviewBlock.h"

#include "core/Nodes.h"
#include "core/MainController.h"


PreviewBlock::PreviewBlock(MainController* controller, QString uid)
    : OneInputBlock(controller, uid)
    , m_valueString("0%")
    , m_additionalValueString(this, "additionalValueString", "")
    , m_currentValue(this, "currentValue", 0, 0, 1)
    , m_valueHistory(50*3)  // 50 FPS * 3s
{
    m_valueHistory.fill(0.0, m_valueHistory.capacity());
    connect(m_controller->engine(), SIGNAL(updateBlocks(double)), this, SLOT(eachFrame()));
}

QVector<double> PreviewBlock::getValueHistory() const {
    return m_valueHistory.toVector();
}

void PreviewBlock::eachFrame() {
    double value = m_inputNode->getValue();
    m_currentValue = value;
    if (m_inputNode->constData().absoluteMaximumIsProvided()) {
        double absValue = m_inputNode->getAbsoluteValue();
        setValueString(QString::number(60.0 / absValue, 'f', 1) + " BPM");
        m_additionalValueString = QString::number(absValue, 'f', 2) + " sec\n"
                + QString::number(1.0 / absValue, 'f', 2) + " Hz ";
    } else {
        setValueString(QString::number(value * 100, 'f', 1) + "%");
        m_additionalValueString = "DMX Value " + QString::number(qRound(value * 255));
    }
    m_valueHistory.push_back(value);
    emit valueHistoryChanged();
}
