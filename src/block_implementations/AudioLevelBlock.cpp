// Copyright (c) 2016 Electronic Theatre Controls, Inc., http://www.etcconnect.com
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "AudioLevelBlock.h"

#include "MainController.h"
#include "Nodes.h"
#include "AudioEngine.h"


AudioLevelBlock::AudioLevelBlock(MainController *controller, QString uid)
    : OneOutputBlock(controller, uid, info().qmlFile)
    , m_analyzer(nullptr)
    , m_currentBand(0.5)
    , m_lastValue(0.0)
    , m_agcEnabled(true)
{
    setInputByName(controller->audioEngine()->getDefaultAnalyzer()->getDeviceName());
}

AudioLevelBlock::~AudioLevelBlock() {
    if (m_analyzer) m_analyzer->removeReference(this);
}

QJsonObject AudioLevelBlock::getState() const {
    QJsonObject state;
    double dp = m_controller->getGuiScaling();
    state["inputName"] = getInputName();
    state["currentBand"] = getCurrentBand();
    state["agcEnabled"] = getAgcEnabled();
    state["guiItemWidth"] = getGuiItemConst()->width() / dp;
    return state;
}

void AudioLevelBlock::setState(const QJsonObject& state) {
    double dp = m_controller->getGuiScaling();
    setInputByName(state["inputName"].toString());
    setCurrentBand(state["currentBand"].toDouble());
    setAgcEnabled(state["agcEnabled"].toBool());
    if (state["guiItemWidth"].toDouble() > 0) {
        getGuiItem()->setWidth(state["guiItemWidth"].toDouble() * dp);
    }
}

void AudioLevelBlock::updateOutput() {
    if (!m_analyzer) return;
    double gain = m_agcEnabled ? m_analyzer->getAgcValue() : 1.0;
    double max = limit(0, m_analyzer->getLevelAtBand(m_currentBand) * gain, 1);
    double value = qMax(m_lastValue - 0.08, max);
    m_outputNode->setValue(value);
    m_lastValue = value;
}

QVector<double> AudioLevelBlock::getSpectrumPoints() {
    if (!m_analyzer) return QVector<double>();
    const auto spectrum = m_analyzer->getSimplifiedSpectrum();
    return QVector<double>::fromStdVector(spectrum);
}

void AudioLevelBlock::setInputByName(QString name) {
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

QString AudioLevelBlock::getInputName() const {
    if (!m_analyzer) return "";
    return m_analyzer->getDeviceName();
}

void AudioLevelBlock::setCurrentBand(double value) {
    m_currentBand = limitToOne(value);
    emit currentBandChanged();
}
