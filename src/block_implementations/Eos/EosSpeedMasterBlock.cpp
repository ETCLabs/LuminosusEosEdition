#include "EosSpeedMasterBlock.h"

#include "core/MainController.h"
#include "core/Nodes.h"


EosSpeedMasterBlock::EosSpeedMasterBlock(MainController* controller, QString uid)
    : OneInputBlock(controller, uid)
    , m_bpm(this, "bpm", 120, 0, 999)
    , m_sendRegularly(this, "sendRegularly", true)
{
    connect(m_inputNode, &NodeBase::dataChanged, this, &EosSpeedMasterBlock::onInputChanged);
    connect(m_controller->eosManager(), &EosOSCManager::connectionEstablished, this, &EosSpeedMasterBlock::sendBpm);

    m_timer.setInterval(1000);
    connect(&m_timer, &QTimer::timeout, [this](){ if (m_sendRegularly) this->sendBpm(); });
    m_timer.start();
}

void EosSpeedMasterBlock::getAdditionalState(QJsonObject& state) const {
    state["effectNumbers"] = serialize(m_effectNumbers);
    state["multipliers"] = serialize(m_multipliers);
}

void EosSpeedMasterBlock::setAdditionalState(const QJsonObject& state) {
    m_effectNumbers = deserialize<QStringList>(state["effectNumbers"].toString());
    m_multipliers = deserialize<QVector<double>>(state["multipliers"].toString());
    emit effectNumbersChanged();
    emit multipliersChanged();
    sendBpm();
}

void EosSpeedMasterBlock::onInputChanged() {
    double value = m_inputNode->getAbsoluteValue();
    if (value <= 0) return;
    double bpm = 60.0 / value;
    if (int(bpm*10) == int(m_bpm*10)) return;
    m_bpm = bpm;
    emit bpmChanged();
    sendBpm();
}

void EosSpeedMasterBlock::addEffect(double effectNumber) {
    QString number = QString::number(effectNumber, 'f', 2);
    if (m_effectNumbers.contains(number)) return;
    m_effectNumbers.append(number);
    m_multipliers.append(1);
    emit effectNumbersChanged();
    emit multipliersChanged();
}

void EosSpeedMasterBlock::removeEffect(int index) {
    if (index < 0 || index >= m_effectNumbers.size()) return;
    m_effectNumbers.removeAt(index);
    m_multipliers.removeAt(index);
    emit effectNumbersChanged();
    emit multipliersChanged();
}

void EosSpeedMasterBlock::setMultiplier(int index, double newValue) {
    if (index < 0 || index >= m_multipliers.size()) return;
    m_multipliers[index] = newValue;
    emit multipliersChanged();
}

double EosSpeedMasterBlock::getMultiplier(int index) const {
    if (index < 0 || index >= m_multipliers.size()) return {};
    return m_multipliers[index];
}

QStringList EosSpeedMasterBlock::getEffectNumbers() const {
    return m_effectNumbers;
}

void EosSpeedMasterBlock::sendBpm() {
    for (int i=0; i<m_effectNumbers.size(); ++i) {
        double effectBpm = m_bpm * getMultiplier(i);
        QString message = "/eos/user/0/newcmd/Effect/%1/BPM/%2/#";
        message = message.arg(m_effectNumbers[i],
                              QString::number(effectBpm, 'f', 1));
        m_controller->lightingConsole()->sendMessage(message);
    }
}
