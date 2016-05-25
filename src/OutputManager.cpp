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

#include <stdint.h>

#include "OutputManager.h"
#include "ArtNetSender.h"
#include "MainController.h"

OutputManager::OutputManager(MainController* controller) : m_artnet(), m_universes(16, std::vector<uint8_t>(512, 0)) {
    connect(controller->engine(), SIGNAL(updateOutput(double)), this, SLOT(triggerOutput()));
}

void OutputManager::setChannel(int channel, float value) {
    channel--; // DMX channel 1 is index 0 in array
    int universe = channel / 512; // integer division
    m_universes[universe][channel] = uint8_t(value * 255);
    m_universe_changed[universe] = true;
}

void OutputManager::setAllChannel(float value) {
    for (int i = 0; i < 512; i++) {
        m_universes[0][i] = uint8_t(value * 255);
    }
    m_universe_changed[0] = true;
}

void OutputManager::triggerOutput() {
    if (m_universe_changed[0]) {
        m_artnet.sendData(m_universes[0]);
        m_universe_changed[0] = false;
    }
}
