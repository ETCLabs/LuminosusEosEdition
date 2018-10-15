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

#include "core/MainController.h"
#include "core/Nodes.h"
#include "audio/AudioEngine.h"


AudioLevelBlock::AudioLevelBlock(MainController *controller, QString uid)
    : OneOutputBlock(controller, uid)
    , m_analyzer(nullptr)
    , m_currentBand(0.5)
    , m_upperBound(0.8)
    , m_bandwidth(0.1)
    , m_lastValue(0.0)
    , m_peakPosition(0.0)
    , m_agcEnabled(true)
    , m_manualGain(1.0)
    , m_frequencyMode(false)
    , m_showSpectrogram(this, "showSpectrogram", false)
    , m_spectrogram(SIMPLIFIED_SPECTRUM_LENGTH, 180, QImage::Format_ARGB32)
    , m_spectrogramPos(0)
{
    m_widthIsResizable = true;

    AudioInputAnalyzer* analyzer = controller->audioEngine()->getDefaultAnalyzer();
    if (analyzer) {
        setInputByName(analyzer->getDeviceName());
    }
    m_controllerFunctionCount = 3;

    m_spectrogram.fill(qRgba(0, 0, 0, 1));
}

AudioLevelBlock::~AudioLevelBlock() {
    if (m_analyzer) m_analyzer->removeReference(this);
}

void AudioLevelBlock::getAdditionalState(QJsonObject& state) const {
    state["inputName"] = getInputName();
    state["currentBand"] = getCurrentBand();
    state["upperBound"] = getUpperBound();
    state["bandwidth"] = getBandwidth();
    state["agcEnabled"] = getAgcEnabled();
    state["manualGain"] = getManualGain();
    state["frequencyMode"] = getFrequencyMode();
}

void AudioLevelBlock::setAdditionalState(const QJsonObject& state) {
    setInputByName(state["inputName"].toString());
    setCurrentBand(state["currentBand"].toDouble());
    setUpperBound(state["upperBound"].toDouble());
    setBandwidth(state["bandwidth"].toDouble());
    setAgcEnabled(state["agcEnabled"].toBool());
    if (state["manualGain"].toDouble() != 0) {
        setManualGain(state["manualGain"].toDouble());
    }
    setFrequencyMode(state["frequencyMode"].toBool());
    readAttributesFrom(state);
}

void AudioLevelBlock::updateOutput() {
    if (m_frequencyMode) {
        updateOutputFrequency();
    } else {
        updateOutputLevel();
    }

    if (m_showSpectrogram) {
        // update spectrogram:

        const auto& spectrum = m_analyzer->getSimplifiedSpectrum();
        const double gain = m_agcEnabled ? m_analyzer->getAgcValue() * 1.1 : m_manualGain;

        // move all pixels one line up (except first line):
        for (int x=0; x < m_spectrogram.width(); ++x) {
            for (int y=1; y < m_spectrogram.height(); ++y) {
                m_spectrogram.setPixel(x, y - 1, m_spectrogram.pixel(x, y));
            }
        }

        // draw new line at the bottom:
        for (int x=0; x < m_spectrogram.width(); ++x) {
            double power = limitToOne(spectrum[x % spectrum.size()] * gain);
            QRgb col = QColor::fromHsvF(0.6, 1.0 - pow(power, 1.8), 1, power).rgba();
            m_spectrogram.setPixel(x, m_spectrogram.height() - 1, col);
        }

        emit spectrogramChanged();
    }

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

double AudioLevelBlock::getManualGainNorm() const {
    return qSqrt((m_manualGain - 0.2) / 7.8);
}

void AudioLevelBlock::setManualGainNorm(double value) {
    m_manualGain = limit(0.2, qPow(value, 2) * 7.8 + 0.2, 8);
    emit manualGainChanged();
}

void AudioLevelBlock::onControllerRotated(double relativeAmount, double, bool) {
    if (m_controllerFunctionSelected == 0) {
        setCurrentBand(getCurrentBand() + relativeAmount);
    } else if(m_controllerFunctionSelected == 1) {
        setUpperBound(getUpperBound() + relativeAmount);
    } else {
        setBandwidth(getBandwidth() + relativeAmount);
    }
}

void AudioLevelBlock::updateOutputLevel() {
    if (!m_analyzer) return;
    const std::vector<double>& spectrum = m_analyzer->getSimplifiedSpectrum();
    const int spectrumSize = spectrum.size();
    const int begin = limit(0, int(spectrumSize * m_currentBand), spectrumSize - 1);
    const int end = limit(1, int(spectrumSize * (m_currentBand + m_bandwidth)), spectrumSize);

    // get max level within selected bands:
    double max = 0.0;
    if (end - begin < 2) {
        // selection is a single band
        max = spectrum[begin];
    } else {
        // selection contains multiple bands
        for (int i=begin; i<end; ++i) {
            max = qMax(max, spectrum[i]);
        }
    }
    // apply AGC if enabled:
    const double gain = m_agcEnabled ? m_analyzer->getAgcValue() : m_manualGain;
    max *= gain;

    // scale value under uppder bound:
    max = limit(0, max / qMax(0.001, m_upperBound), 1);

    // smooth decrease of level:
    const double value = qMax(m_lastValue - 0.08, max);

    // update node value:
    m_outputNode->setValue(value);
    m_lastValue = value;
}

void AudioLevelBlock::updateOutputFrequency() {
    if (!m_analyzer) return;
    const std::vector<double>& spectrum = m_analyzer->getSimplifiedSpectrum();
    const int spectrumSize = spectrum.size();
    const int begin = limit(0, int(spectrumSize * m_currentBand), spectrumSize - 1);
    const int end = limit(1, int(spectrumSize * (m_currentBand + m_bandwidth)), spectrumSize);
    if (end - begin < 2) return;  // band is too small

    const double gain = m_agcEnabled ? m_analyzer->getAgcValue() : m_manualGain;

    // get peak position within selected bands:
    int peakIndex = -1;
    for (int i=begin; i<end; ++i) {
        // check if above upper limit -> if yes this is the peak
        if (spectrum[i] * gain >= m_upperBound) {
            peakIndex = i;
            break;
        }
    }

    if (peakIndex < begin) {
        // spectrum doesn't reach upper bound
        return;
    }

    // calculate relative position of peak:
    const double position = double(peakIndex - begin) / (end - begin);
    //m_peakPosition = double(peakIndex) / spectrumSize;
    m_peakPosition = position;
    emit peakPositionChanged();

    // update node value:
    m_outputNode->setValue(limit(0, position, 1));
}
