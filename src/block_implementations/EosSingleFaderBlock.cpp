#include "EosSingleFaderBlock.h"

#include "MainController.h"
#include "Nodes.h"


EosSingleFaderBlock::EosSingleFaderBlock(MainController* controller, QString uid)
    : OneInputBlock(controller, uid, info().qmlFile)
    , m_bankIndex(QString::number(controller->eosManager()->getNewFaderBankNumber()))
    , m_page(1)
    , m_faderNumber(1)
    , m_bankLabel("")
    , m_faderLabels(10)
    , m_faderLevels(10)
    , m_stopNode(nullptr)
    , m_fireNode(nullptr)
    , m_lastStopNodeValue(0)
    , m_lastFireNodeValue(0)
{
    // prepare additional nodes:
    m_stopNode = createInputNode("stopNode");
    m_fireNode = createInputNode("fireNode");
    connect(m_inputNode, SIGNAL(dataChanged()), this, SLOT(onValueChanged()));
    connect(m_stopNode, SIGNAL(dataChanged()), this, SLOT(onStopValueChanged()));
    connect(m_fireNode, SIGNAL(dataChanged()), this, SLOT(onFireValueChanged()));

    connect(m_controller->eosManager(), SIGNAL(connectionEstablished()),
            this, SLOT(onEosConnectionEstablished()));
    connect(controller->eosManager(), SIGNAL(eosMessageReceived(EosOSCMessage)),
            this, SLOT(onIncomingEosMessage(EosOSCMessage)));
    connect(controller->eosManager(), SIGNAL(connectionReset()),
            this, SLOT(onConnectionReset()));

    sendConfigMessage();
}

QJsonObject EosSingleFaderBlock::getState() const {
    QJsonObject state;
    state["page"] = getPage();
    state["faderNumber"] = getFaderNumber();
    return state;
}

void EosSingleFaderBlock::setState(const QJsonObject& state) {
    setPageFromGui(state["page"].toInt());
    setFaderNumber(state["faderNumber"].toInt());
}

void EosSingleFaderBlock::onValueChanged() {
    double value = m_inputNode->getValue();
    if (value == getFaderLevel()) return;
    setFaderLevelFromGui(value);
}

void EosSingleFaderBlock::onStopValueChanged() {
    double value = m_stopNode->getValue();
    double threshold = LuminosusConstants::triggerThreshold;
    if (value >= threshold && m_lastStopNodeValue < threshold) {
        sendStopEvent(true);
    } else if (value < threshold && m_lastStopNodeValue >= threshold) {
        sendStopEvent(false);
    }
    m_lastStopNodeValue = value;
}

void EosSingleFaderBlock::onFireValueChanged() {
    double value = m_fireNode->getValue();
    double threshold = LuminosusConstants::triggerThreshold;
    if (value >= threshold && m_lastFireNodeValue < threshold) {
        sendFireEvent(true);
    } else if (value < threshold && m_lastFireNodeValue >= threshold) {
        sendFireEvent(false);
    }
    m_lastFireNodeValue = value;
}

void EosSingleFaderBlock::setPageFromGui(int value) {
    if (value == m_page) return;
    m_page = limit(1, value, getMaxFaderPage());
    sendConfigMessage();
    emit pageChanged();
}

void EosSingleFaderBlock::setFaderNumber(int value) {
    m_faderNumber = limit(1, value, 10);
    emit faderNumberChanged();
    emit faderLabelChanged();
    emit faderLevelChanged();
}

void EosSingleFaderBlock::setFaderLabelFromOsc(int faderIndex, QString label) {
    if (faderIndex < 0 || faderIndex > 9) return;
    m_faderLabels[faderIndex] = label;
    if (faderIndex == m_faderNumber - 1) {
        emit faderLabelChanged();
    }
}

void EosSingleFaderBlock::setFaderLevelFromGui(qreal value) {
    value = limit(0, value, 1);
    m_faderLevels[m_faderNumber - 1] = value;
    // emitting faderLevelChanged() to update GUI because call could come from inputNode
    emit faderLevelChanged();

    QString message = "/eos/fader/%1/%2";
    message = message.arg(m_bankIndex, QString::number(m_faderNumber));
    m_controller->eosConnection()->sendMessage(message, value);
}

void EosSingleFaderBlock::setFaderLevelFromOsc(int faderIndex, qreal value){
    if (faderIndex < 0 || faderIndex > 9) return;
    m_faderLevels[faderIndex] = limit(0, value, 1);
    if (faderIndex == m_faderNumber - 1) {
        emit faderLevelChanged();
    }
}

void EosSingleFaderBlock::sendLoadEvent(bool value) {
    QString message = "/eos/fader/%1/%2/load=";
    message = message.arg(m_bankIndex, QString::number(m_faderNumber));
    message.append(value ? "1.0": "0.0");
    m_controller->eosConnection()->sendMessage(message);
}

void EosSingleFaderBlock::sendUnloadEvent() {
    QString message = "/eos/fader/%1/%2/unload";
    message = message.arg(m_bankIndex, QString::number(m_faderNumber));
    m_controller->eosConnection()->sendMessage(message);
}

void EosSingleFaderBlock::sendStopEvent(bool value) {
    QString message = "/eos/fader/%1/%2/stop=";
    message = message.arg(m_bankIndex, QString::number(m_faderNumber));
    message.append(value ? "1.0": "0.0");
    m_controller->eosConnection()->sendMessage(message);
}

void EosSingleFaderBlock::sendFireEvent(bool value) {
    QString message = "/eos/fader/%1/%2/fire=";
    message = message.arg(m_bankIndex, QString::number(m_faderNumber));
    message.append(value ? "1.0": "0.0");
    m_controller->eosConnection()->sendMessage(message);
}

void EosSingleFaderBlock::onEosConnectionEstablished() {
    sendConfigMessage();
}

void EosSingleFaderBlock::sendConfigMessage() {
    // send /eos/fader/<index>/config/<page>/10 command:
    QString message = "/eos/fader/%1/config/%2/10";
    message = message.arg(m_bankIndex, QString::number(m_page));
    m_controller->eosConnection()->sendMessage(message);
}

void EosSingleFaderBlock::onIncomingEosMessage(const EosOSCMessage& msg) {
    // check if this message belongs to this FaderBank:
    if (msg.pathPart(0) != "fader" || msg.pathPart(1) != m_bankIndex) {
        // it doesn't -> ignore it:
        return;
    }

    if (msg.path().size() == 2 && msg.arguments().size() >= 1) {
        // this message contains a descriptive label for this fader bank
        // most of the time it is the number of the page
        // /eos/out/fader/<index>=<label>
        setBankLabel(msg.stringValue());
        int page = msg.stringValue().toInt();
        if (page >= 1 && page <= 100) {
            m_page = page;
            emit pageChanged();
        }
    } else if (msg.pathPart(3) == "name") {
        // this message contains the label for a specific fader in this bank
        // /eos/out/fader/<index>/<faderNumber>/name=<label>
        int faderNumber = msg.pathPart(2).toInt();
        QString label = msg.stringValue();
        setFaderLabelFromOsc(faderNumber - 1, label);
    } else if ( msg.path().size() == 3) {
        // this message contains the current level of a fader
        // /eos/fader/<index>/<fader>=<level> (not /eos/out/ !)
        int faderNumber = msg.pathPart(2).toInt();
        double level = msg.numericValue();
        setFaderLevelFromOsc(faderNumber - 1, level);
    }
}

void EosSingleFaderBlock::onConnectionReset() {
    sendConfigMessage();
}

int EosSingleFaderBlock::getMaxFaderPage() {
    QString eosVersion = m_controller->eosManager()->getConsoleVersion();
    if (eosVersion.startsWith("2.4.")) {
        return 100;
    } else {
        return 30;
    }
}
