#include "BeatDetectionBlock.h"

#include "core/MainController.h"
#include "core/Nodes.h"
#include "audio/AudioEngine.h"


BeatDetectionBlock::BeatDetectionBlock(MainController *controller, QString uid)
    : OneOutputBlock(controller, uid)
    , m_analyzer(nullptr)
    , m_minBpm(75)
    , m_factor(1)
{
    AudioInputAnalyzer* analyzer = controller->audioEngine()->getDefaultAnalyzer();
    if (analyzer) {
        setInputByName(analyzer->getDeviceName());
    }
}

BeatDetectionBlock::~BeatDetectionBlock() {
    if (m_analyzer) m_analyzer->removeReferenceForBpm(this);
}

void BeatDetectionBlock::getAdditionalState(QJsonObject& state) const {
    state["inputName"] = getInputName();
    state["minBpm"] = getMinBpm();
    state["factor"] = getFactor();
}

void BeatDetectionBlock::setAdditionalState(const QJsonObject& state) {
    setInputByName(state["inputName"].toString());
    setMinBpm(state["minBpm"].toInt());
    setFactor(state["factor"].toDouble());
}

void BeatDetectionBlock::updateOutput() {
    if (!m_analyzer) return;
    double realBpm = m_analyzer->getBpm(m_minBpm) * m_factor;
    if (realBpm <= 0) return;
    m_outputNode->setAbsoluteValue(60.0 / realBpm);
}

void BeatDetectionBlock::onBpmChanged() {
    emit bpmChanged();
    if (m_analyzer->getBpmIsValid()) {
        updateOutput();
    }
}

void BeatDetectionBlock::setInputByName(QString name) {
    if (m_analyzer && m_analyzer->getDeviceName() == name) return;
    AudioInputAnalyzer* newAnalyzer = m_controller->audioEngine()->getAnalyzerByName(name);
    if (!newAnalyzer) return;
    if (m_analyzer) {
        disconnect(m_analyzer, SIGNAL(bpmChanged()), this, SLOT(onBpmChanged()));
        m_analyzer->removeReferenceForBpm(this);
    }
    m_analyzer = newAnalyzer;
    m_analyzer->addReferenceForBpm(this);
    connect(m_analyzer, SIGNAL(bpmChanged()), this, SLOT(onBpmChanged()));
    emit inputChanged();
    emit bpmChanged();
}

void BeatDetectionBlock::setFactor(double value) {
    m_factor = limit(0.1, value, 10);
    emit factorChanged();
    updateOutput();
}

QString BeatDetectionBlock::getInputName() const {
    if (!m_analyzer) return "";
    return m_analyzer->getDeviceName();
}
