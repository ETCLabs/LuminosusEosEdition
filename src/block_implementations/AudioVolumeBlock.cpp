#include "AudioVolumeBlock.h"

#include "MainController.h"
#include "Nodes.h"
#include "AudioEngine.h"


AudioVolumeBlock::AudioVolumeBlock(MainController *controller, QString uid)
    : OneOutputBlock(controller, uid, info().qmlFile)
    , m_analyzer(nullptr)
    , m_fluxNode(nullptr)
    , m_agcEnabled(true)
{
    m_fluxNode = createOutputNode("fluxNode");
    setInputByName(controller->audioEngine()->getDefaultAnalyzer()->getDeviceName());
}

AudioVolumeBlock::~AudioVolumeBlock() {
    if (m_analyzer) m_analyzer->removeReference(this);
}

QJsonObject AudioVolumeBlock::getState() const {
    QJsonObject state;
    state["inputName"] = getInputName();
    state["agcEnabled"] = getAgcEnabled();
    return state;
}

void AudioVolumeBlock::setState(const QJsonObject& state) {
    setInputByName(state["inputName"].toString());
    setAgcEnabled(state["agcEnabled"].toBool());
}

void AudioVolumeBlock::updateOutput() {
    if (!m_analyzer) return;
    double volumeGain = m_agcEnabled ? m_analyzer->getAgcValue() : 1.0;
    double fluxGain = m_agcEnabled ? m_analyzer->getSpectralFluxAgcValue() : 1.0;
    m_outputNode->setValue(limit(0, m_analyzer->getMaxLevel() * volumeGain, 1));
    m_fluxNode->setValue(limit(0, m_analyzer->getCurrentSpectralFlux() * fluxGain, 1));
    emit volumeChanged();
}

void AudioVolumeBlock::setInputByName(QString name) {
    if (m_analyzer && m_analyzer->getDeviceName() == name) return;
    AudioInputAnalyzer* newAnalyzer = m_controller->audioEngine()->getAnalyzerByName(name);
    if (!newAnalyzer) return;
    if (m_analyzer) {
        disconnect(m_analyzer, SIGNAL(spectrumChanged()), this, SLOT(updateOutput()));
        m_analyzer->removeReference(this);
    }
    m_analyzer = newAnalyzer;
    m_analyzer->addReference(this);
    connect(m_analyzer, SIGNAL(spectrumChanged()), this, SLOT(updateOutput()));
    emit inputChanged();
}

QString AudioVolumeBlock::getInputName() const {
    if (!m_analyzer) return "";
    return m_analyzer->getDeviceName();
}
