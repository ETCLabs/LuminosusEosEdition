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

#include "OutputManager.h"

#include <stdint.h>

#include "core/MainController.h"


OutputManager::OutputManager(MainController* controller)
    : QObject(controller)
    , m_sAcnEnabled(false)
    , m_artnetEnabled(false)
    , m_sAcnStartUniverse(1)
    , m_sAcnPriority(100)
    , m_artnetNet(0)
    , m_artnetSubnet(0)
    , m_broadcastArtnet(false)
    , m_artnet(m_artnetNet, m_artnetSubnet)
    //, m_artnetDiscoveryManager()
    , m_sAcnSender(m_sAcnStartUniverse, 100)
    , m_universes(16, QVector<uint8_t>(512, 0))
    , m_usedAddressCount(0)
    , m_nextAddressToUse(1)
{
    for (int i=0; i<16; ++i) {
        m_universeDirty[i] = false;
    }
    connect(controller->engine(), SIGNAL(updateOutput(double)), this, SLOT(triggerOutput()));
    //connect(&m_artnetDiscoveryManager, SIGNAL(discoveredNodesChanged()), this, SIGNAL(discoveredNodesChanged()));
}

QJsonObject OutputManager::getState() const {
    QJsonObject state;
    state["sAcnEnabled"] = getSAcnEnabled();
    state["artnetEnabled"] = getArtnetEnabled();
    state["sAcnStartUniverse"] = getSAcnStartUniverse();
    state["sAcnPriority"] = getSAcnPriority();
    state["artnetNet"] = getArtnetNet();
    state["artnetSubnet"] = getArtnetSubnet();
    state["broadcastArtnet"] = getBroadcastArtnet();
    return state;
}

void OutputManager::setState(const QJsonObject& state) {
    if (state.isEmpty()) return;
    setSAcnEnabled(state["sAcnEnabled"].toBool());
    setArtnetEnabled(state["artnetEnabled"].toBool());
    setSAcnStartUniverse(state["sAcnStartUniverse"].toInt());
    if (state["sAcnPriority"].toInt() > 0) {
        setSAcnPriority(state["sAcnPriority"].toInt());
    }
    setArtnetNet(state["artnetNet"].toInt());
    setArtnetSubnet(state["artnetSubnet"].toInt());
    setBroadcastArtnet(state["broadcastArtnet"].toBool());
}

void OutputManager::setChannel(int address, double value) {
    if (address > 8192 || address < 1) return;
    --address; // DMX channel 1 is index 0 in array
    unsigned int universe = static_cast<unsigned int>(address) / 512; // integer division
    m_universes[universe][static_cast<unsigned int>(address % 512)] = uint8_t(value * 255);
    m_universeDirty[universe] = true;
}

void OutputManager::triggerOutput() {
    for (int universe = 0; universe < 16; ++universe) {
        if (m_universeDirty[universe]) {
            if (m_artnetEnabled) {
                if (m_broadcastArtnet) {
                    //m_artnet.sendUniverseBroadcast(universe, m_universes[universe]);
                } else {
                    //m_artnet.sendUniverseUnicast(universe, m_universes[universe], m_artnetDiscoveryManager.getUnicastAddresses());
                }
            }
            if (m_sAcnEnabled) {
                //m_sAcnSender.sendUniverseMulticast(universe, m_universes[universe]);
            }
            m_universeDirty[universe] = false;
        }
    }
}

int OutputManager::getUnusedAddress(int footprint) {
    int address = m_nextAddressToUse + m_usedAddressCount;
    if (address > 8192) {
        address %= 8192;
    }
    m_usedAddressCount += footprint;
    return address;
}

void OutputManager::setNextAddressToUse(int address) {
    m_nextAddressToUse = address;
    m_usedAddressCount = 0;
}

void OutputManager::setSAcnStartUniverse(int value) {
    m_sAcnStartUniverse = limit(1, value, 63999 - 16);
    m_sAcnSender.setStartUniverse(m_sAcnStartUniverse);
    emit sAcnStartUniverseChanged();
}

void OutputManager::setSAcnPriority(int value) {
    m_sAcnPriority = limit(1, value, 200);
    m_sAcnSender.setPriority(m_sAcnPriority);
    emit sAcnPriorityChanged();
}

void OutputManager::setArtnetNet(int value) {
    m_artnetNet = limit(0, value, 127);
    m_artnet.setNetAndSubnet(m_artnetNet, m_artnetSubnet);
    emit artnetNetChanged();
}

void OutputManager::setArtnetSubnet(int value) {
    m_artnetSubnet = limit(0, value, 15);
    m_artnet.setNetAndSubnet(m_artnetNet, m_artnetSubnet);
    emit artnetSubnetChanged();
}

QVariantList OutputManager::getDiscoveredNodes() {
    //return m_artnetDiscoveryManager.getDiscoveredNodes();
    return QVariantList();
}

QVariantList OutputManager::getDiscoveredLuminosusInstances() {
//    QVariantList instances;
//    for (QVariant node: m_artnetDiscoveryManager.getDiscoveredNodes()) {
//        QVariantMap nodeInfo = node.toMap();
//        if (nodeInfo["longName"].toString().contains("Luminosus")) {
//            instances.append(nodeInfo);
//        }
//    }
//    return instances;
    return QVariantList();
}
