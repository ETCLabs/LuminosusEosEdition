#include "PresentationSlideDmxBlock.h"

#include "core/MainController.h"
#include "core/manager/KeyboardEmulator.h"
#include "core/Nodes.h"

#include <QQuickWindow>


PresentationSlideDmxBlock::PresentationSlideDmxBlock(MainController* controller, QString uid)
    : OneInputBlock(controller, uid)
    , m_lastSlide(0)
    , m_sequenceRunning(false)
{
    connect(m_inputNode, &NodeBase::dataChanged, this, &PresentationSlideDmxBlock::onInputChanged);
}

void PresentationSlideDmxBlock::onInputChanged() {
    int dmxValue = qRound(m_inputNode->getValue() * 255);
    if (dmxValue > 0 && dmxValue != m_lastSlide) {
        qDebug() << "Go to slide" << dmxValue;
        goToSlide(dmxValue);
        m_lastSlide = dmxValue;
    }
}

void PresentationSlideDmxBlock::goToSlide(int number) {
    if (m_sequenceRunning) {
        qDebug() << "Presentation Control: previous key sequence is still running";
        return;
    }
    if (m_controller->guiManager()->getMainWindow()->isActive()) {
        // PowerPoint is not in foreground (but Luminosus)
        // -> don't send key
        return;
    }
    m_sequenceRunning = true;
    QByteArray chars = QString::number(number).toLatin1();
    if (chars.length() <= 0) return;
    QVector<quint32> scanCodes;
    for (char c: chars) {
        switch (c) {
        case '0': scanCodes.append(ScanCode::NUMBER_0); break;
        case '1': scanCodes.append(ScanCode::NUMBER_1); break;
        case '2': scanCodes.append(ScanCode::NUMBER_2); break;
        case '3': scanCodes.append(ScanCode::NUMBER_3); break;
        case '4': scanCodes.append(ScanCode::NUMBER_4); break;
        case '5': scanCodes.append(ScanCode::NUMBER_5); break;
        case '6': scanCodes.append(ScanCode::NUMBER_6); break;
        case '7': scanCodes.append(ScanCode::NUMBER_7); break;
        case '8': scanCodes.append(ScanCode::NUMBER_8); break;
        case '9': scanCodes.append(ScanCode::NUMBER_9); break;
        default: break;
        }
    }
    pressNextKey(scanCodes, 0);
}

void PresentationSlideDmxBlock::pressNextKey(QVector<quint32> scanCodes, int index) {
    if (scanCodes.length() <= index) {
        m_controller->keyboardEmulator()->pressKey(ScanCode::RETURN);
        QTimer::singleShot(50, [this](){
            m_controller->keyboardEmulator()->releaseKey(ScanCode::RETURN);
            m_sequenceRunning = false;
        });
        return;
    }
    m_controller->keyboardEmulator()->pressKey(static_cast<quint32>(scanCodes.at(index)));
    QTimer::singleShot(50, [this, scanCodes, index](){ m_controller->keyboardEmulator()->releaseKey(static_cast<quint32>(scanCodes.at(index))); });
    QTimer::singleShot(100, [this, scanCodes, index](){ pressNextKey(scanCodes, index + 1); });
}
