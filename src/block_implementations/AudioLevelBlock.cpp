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
#include "NodeBase.h"

AudioLevelBlock::AudioLevelBlock(MainController *controller, QString uid) : OneOutputBlock(controller, uid, info().qmlFile) {
	analyzer = controller->audioEngine()->getDefaultInput();
    analyzer->addReference(this);
    connect(controller->engine(), SIGNAL(updateBlocks(double)), this, SLOT(refreshOutput()));
}

AudioLevelBlock::~AudioLevelBlock() {
    if (analyzer) analyzer->removeReference(this);
}

QJsonObject AudioLevelBlock::getState() const {
    QJsonObject state;
    state["inputName"] = analyzer->deviceName;
    return state;
}

void AudioLevelBlock::setInputByName(QString name) {
	if (!mapContains(m_controller->audioEngine()->audioInputs, name)) {
        return;
    }
    if (analyzer) analyzer->removeReference(this);
	analyzer = m_controller->audioEngine()->audioInputs[name];
    analyzer->addReference(this);
    inputChanged();
}

QString AudioLevelBlock::getInputName() { return analyzer->deviceName; }

void AudioLevelBlock::refreshOutput() {
    double max = analyzer->getLevelAtBand(currentBand);
    m_outputNode->setValue(std::max(m_outputNode->m_data->getValue() - 0.08, max));
}

QList<qreal> AudioLevelBlock::getSpectrumPoints() {
    const auto spectrum = analyzer->getSimplifiedSpectrum();
    QList<qreal> points = QList<qreal>::fromVector(QVector<qreal>::fromStdVector(spectrum));
    return points;
}
