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

#include <QDebug>
#include "AudioEngine.h"
#include "MainController.h"

void AudioEngine::initInputs() {
    QList<QAudioDeviceInfo> devices = QAudioDeviceInfo::availableDevices(QAudio::Mode::AudioInput);
    for (auto device: devices) {
        if (device.deviceName().contains("CARD")) continue;
        if (device.deviceName().contains("default")) continue;
        audioInputs[device.deviceName()] = new AudioInputAnalyzer(device, controller);
    }
    if (audioInputs.size() == 0) {
        audioInputs["No Input"] = new AudioInputAnalyzer(controller);
    }
}


void AudioInputAnalyzer::calculateWindows() {
    // Hann Window function
    for (int i=0; i<LONG_NUM_SAMPLES; ++i) {
        longWindow[i] = 0.5f * (1 - qCos((2 * M_PI * i) / (LONG_NUM_SAMPLES - 1)));
    }
    for (int i=0; i<SHORT_NUM_SAMPLES; ++i) {
        shortWindow[i] = 0.5f * (1 - qCos((2 * M_PI * i) / (SHORT_NUM_SAMPLES - 1)));
    }
}

void AudioInputAnalyzer::addReference(void *ref) {
    if (referenceList.size() == 0) {
        connect(audioIODevice, SIGNAL(readyRead()), this, SLOT(audioDataReady()));
        connect(controller->engine(), SIGNAL(updateBlocks(double)), this, SLOT(updateSpectrum()));
    }
    referenceList.insert(ref);
}

void AudioInputAnalyzer::removeReference(void *ref) {
    referenceList.erase(ref);
    if (referenceList.size() == 0) {
        disconnect(audioIODevice, SIGNAL(readyRead()), this, SLOT(audioDataReady()));
        disconnect(controller->engine(), SIGNAL(updateBlocks(double)), this, SLOT(updateSpectrum()));
    }
}

void AudioInputAnalyzer::initAudio(QAudioDeviceInfo info) {
	for (int i=0; i < circBuffer.capacity(); ++i) {
        circBuffer.push_back(0.0);
    }
    QAudioFormat format;
    // Set up the desired format, for example:
    format.setSampleRate(44100);
    format.setChannelCount(1);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::SignedInt);

    if (!info.isFormatSupported(format)) {
        qWarning() << "Default audio format not supported, trying to use the nearest.";
        format = info.nearestFormat(format);
    }
    audioFormat = format;

    audioInput = new QAudioInput(info, format, this);
    audioIODevice = audioInput->start();
}

void AudioInputAnalyzer::audioDataReady() {
    QByteArray data = audioIODevice->readAll();
    const int bytesPerSample = audioFormat.sampleSize() * audioFormat.channelCount() / 8;
    const int numSamples = data.size() / bytesPerSample;
    const char *ptr = data.constData();
    for (int i=0; i<numSamples; ++i) {
        const int16_t pcmSample = *reinterpret_cast<const int16_t*>(ptr);
        // Scale down to range [-1.0, 1.0]
        const float scaled = float(pcmSample) / 32768;
        circBuffer.push_back(scaled);
        ptr += bytesPerSample;
    }
}

void AudioInputAnalyzer::updateSpectrum() {
    for (int i=0; i < LONG_NUM_SAMPLES; ++i) {
        longBuffer[i] = circBuffer[i] * longWindow[i];
    }
    longFftreal.do_fft(longFftOutput.data(), longBuffer.data());
    for (int i=0; i < LONG_NUM_SAMPLES / 2; ++i) {
        const float real = longFftOutput[i];
        const float img = longFftOutput[LONG_NUM_SAMPLES / 2 + i];
        //const float magnitudeInDb = 4.342f * std::log10(real*real + img*img);  // simplified from 20*log10(sqrt(r*r+i*i)) -> dB of Magnitude
        const float magnitude = std::pow(real*real + img*img, 0.2) / 14;  // 14 is most of the time the max value
        longSpectrum[i] = std::max(0.0f, std::min(magnitude, 1.0f));
    }
    float max = 0.0;
    const int offset = circBuffer.size() - SHORT_NUM_SAMPLES;
    for (int i=0; i < SHORT_NUM_SAMPLES; ++i) {
        shortBuffer[i] = circBuffer[offset + i] * shortWindow[i];
    }
    shortFftreal.do_fft(shortFftOutput.data(), shortBuffer.data());
    for (int i=0; i < SHORT_NUM_SAMPLES / 2; ++i) {
        const float real = shortFftOutput[i];
        const float img = shortFftOutput[SHORT_NUM_SAMPLES / 2 + i];
        //const float magnitudeInDb = 4.342f * std::log10(real*real + img*img);  // simplified from 20*log10(sqrt(r*r+i*i)) -> dB of Magnitude
        const float magnitude = std::pow(real*real + img*img, 0.2) / 9;  // 9 is most of the time the max value
        max = std::max(max, magnitude);
        shortSpectrum[i] = std::max(0.0f, std::min(magnitude, 1.0f));
    }
    maxLevel = std::max(0.0f, std::min(max, 1.0f));
    updateSimplifiedSpectrum();
}

void AudioInputAnalyzer::updateSimplifiedSpectrum() {
    int beginFreq = 10;
    double factor = 1.061989883394314;  // pow(22050 / 10., 1/128.)
    double freq = beginFreq;
    for (int i = 0; i<128; ++i) {
        double nextFreq = 10 * std::pow(factor, i+1);
        if (freq < 200) {
            const std::size_t startIndex = freq / 22050 * (LONG_NUM_SAMPLES / 2);
            const std::size_t endIndex = nextFreq / 22050 * (LONG_NUM_SAMPLES / 2);
            const std::size_t valuesTillNext = endIndex - startIndex;
            float max = longSpectrum[startIndex];
            for (std::size_t j=1; j < valuesTillNext; ++j) {
                max = std::max(max, longSpectrum[startIndex+j]);
            }
            simplifiedSpectrum[i] = max;
        } else {
            const std::size_t startIndex = freq / 22050 * (SHORT_NUM_SAMPLES / 2);
            const std::size_t endIndex = nextFreq / 22050 * (SHORT_NUM_SAMPLES / 2);
            const std::size_t valuesTillNext = endIndex - startIndex;
            float max = shortSpectrum[startIndex];
            for (std::size_t j=1; j < valuesTillNext; ++j) {
                max = std::max(max, shortSpectrum[startIndex+j]);
            }
            simplifiedSpectrum[i] = max;
        }
        freq = nextFreq;
    }
    //simplifiedSpectrum[50] = 1;  // point between long and short spectrum (200 Hz)
}
