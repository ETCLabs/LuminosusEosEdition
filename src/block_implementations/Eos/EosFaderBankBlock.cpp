#include "EosFaderBankBlock.h"

#include "core/MainController.h"


EosFaderBankBlock::EosFaderBankBlock(MainController* controller, QString uid)
    : BlockBase(controller, uid)
    , m_bankIndex(QString::number(controller->eosManager()->getNewFaderBankNumber()))
    , m_page(1)
    , m_bankLabel("")
    , m_faderLabels(10)
    , m_faderLevels(10)
    , m_externalLevels(10)
    , m_externalLevelsValid(10, false)
    , m_faderSync(10, false)
    , m_catchFaders(this, "catchFaders", true)
{
    connect(m_controller->eosManager(), SIGNAL(connectionEstablished()),
            this, SLOT(onEosConnectionEstablished()));
    connect(controller->eosManager(), SIGNAL(eosMessageReceived(EosOSCMessage)),
            this, SLOT(onIncomingEosMessage(EosOSCMessage)));
    connect(controller->eosManager(), SIGNAL(connectionReset()),
            this, SLOT(onConnectionReset()));

    m_lastExtTime.start();
    sendConfigMessage();
}

void EosFaderBankBlock::getAdditionalState(QJsonObject& state) const {
    state["page"] = getPage();
}

void EosFaderBankBlock::setAdditionalState(const QJsonObject& state) {
    setPageFromGui(state["page"].toInt());
}

void EosFaderBankBlock::setPageFromGui(int value) {
    if (value == m_page) return;
    m_page = limit(1, value, getMaxFaderPage());
    m_faderLevels.fill(0.0);
    m_faderSync.fill(false);
    emit faderLevelsChanged();
    sendConfigMessage();
    emit pageChanged();
}

void EosFaderBankBlock::setFaderLabelFromOsc(int faderIndex, QString label) {
    if (faderIndex < 0 || faderIndex > 9) return;
    m_faderLabels[faderIndex] = label;
    emit faderLabelsChanged();
}

void EosFaderBankBlock::setFaderLevel(int faderIndex, qreal value) {
    if (faderIndex < 0 || faderIndex > 9) return;
    value = limit(0, value, 1);

    m_faderLevels[faderIndex] = value;

    QString message = "/eos/user/1/fader/%1/%2";
    message = message.arg(m_bankIndex, QString::number(faderIndex + 1));
    m_controller->lightingConsole()->sendMessage(message, value);
}

void EosFaderBankBlock::setFaderLevelFromGui(int faderIndex, qreal value) {
    m_faderSync[faderIndex] = false;
    setFaderLevel(faderIndex, value);

    // emitting faderLevelsChanged() not actually neccessary
    // because value in GUI is already up-to-date?
}

void EosFaderBankBlock::setFaderLevelFromExt(int faderIndex, qreal value) {
    if (m_catchFaders) {
        if (!m_faderSync[faderIndex]) {
            if (m_externalLevelsValid[faderIndex]) {
                if ((m_externalLevels[faderIndex] <= m_faderLevels[faderIndex] && (value + 0.01) >= m_faderLevels[faderIndex])
                        || (m_externalLevels[faderIndex] >= m_faderLevels[faderIndex] && (value - 0.01) <= m_faderLevels[faderIndex]))
                    m_faderSync[faderIndex] = true;
            }
            if (abs(value - m_faderLevels[faderIndex]) < 0.02) {
                m_faderSync[faderIndex] = true;
            }
        }
        m_externalLevels[faderIndex] = value;
        m_externalLevelsValid[faderIndex] = true;

        if (!m_faderSync[faderIndex])
            return;
    }

    setFaderLevel(faderIndex, value);
    m_lastExtTime.restart();
    emit faderLevelsChanged();
}

void EosFaderBankBlock::setFaderLevelFromOsc(int faderIndex, qreal value){
    if (faderIndex < 0 || faderIndex > 9) return;
    m_faderLevels[faderIndex] = limit(0, value, 1);

    // asynchronous osc feedback can break sync so give it some time
    if (abs(value - m_externalLevels[faderIndex]) > 0.04 && m_lastExtTime.elapsed() > 200)
        m_faderSync[faderIndex] = false;

    emit faderLevelsChanged();
}

void EosFaderBankBlock::sendLoadEvent(int faderIndex, bool value) {
    QString message = "/eos/user/1/fader/%1/%2/load";
    message = message.arg(m_bankIndex, QString::number(faderIndex + 1));
    m_controller->lightingConsole()->sendMessage(message, value ? 1.0 : 0.0);
}

void EosFaderBankBlock::sendUnloadEvent(int faderIndex) {
    QString message = "/eos/user/1/fader/%1/%2/unload";
    message = message.arg(m_bankIndex, QString::number(faderIndex + 1));
    m_controller->lightingConsole()->sendMessage(message);
}

void EosFaderBankBlock::sendStopEvent(int faderIndex, bool value) {
    QString message = "/eos/user/1/fader/%1/%2/stop";
    message = message.arg(m_bankIndex, QString::number(faderIndex + 1));
    m_controller->lightingConsole()->sendMessage(message, value ? 1.0 : 0.0);
}

void EosFaderBankBlock::sendFireEvent(int faderIndex, bool value) {
    QString message = "/eos/user/1/fader/%1/%2/fire";
    message = message.arg(m_bankIndex, QString::number(faderIndex + 1));
    m_controller->lightingConsole()->sendMessage(message, value ? 1.0 : 0.0);
}

void EosFaderBankBlock::sendPageMinusEvent() {
    if (m_page == 1) {
        setPageFromGui(getMaxFaderPage());
    } else {
        setPageFromGui(m_page - 1);
    }
}

void EosFaderBankBlock::sendPagePlusEvent() {
    if (m_page == getMaxFaderPage()) {
        setPageFromGui(1);
    } else {
        setPageFromGui(m_page + 1);
    }
}

void EosFaderBankBlock::onEosConnectionEstablished() {
    sendConfigMessage();
}

void EosFaderBankBlock::sendConfigMessage() {
    // send /eos/fader/<index>/config/<page>/10 command:
    QString message = "/eos/user/1/fader/%1/config/%2/10";
    message = message.arg(m_bankIndex, QString::number(m_page));
    m_controller->lightingConsole()->sendMessage(message);
}

void EosFaderBankBlock::onIncomingEosMessage(const EosOSCMessage& msg) {
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

void EosFaderBankBlock::onConnectionReset() {
    sendConfigMessage();
}

int EosFaderBankBlock::getMaxFaderPage() {
    QString eosVersion = m_controller->eosManager()->getConsoleVersion();
    if (eosVersion.startsWith("2.4.")) {
        return 100;
    } else {
        return 30;
    }
}
