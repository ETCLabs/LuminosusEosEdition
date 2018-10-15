#include "VirtualFixtureBlock.h"

#include "core/Nodes.h"
#include "sacn/sacnlistener.h"
#include "utils.h"


VirtualFixtureBlock::VirtualFixtureBlock(MainController* controller, QString uid)
    : OneOutputBlock (controller, uid)
    , m_listener(nullptr)
    , m_universe(this, "universe", 1, 1, 63999)
    , m_channel(this, "channel", 1, 1, 512)
    , m_numChannels(this, "numChannels", 1, 1, 50)
    , m_actualChannelCount(this, "actualChannelCount", 1, 1, 50, /*persistent*/ false)
{
    updateListener();
    connect(&m_universe, &IntegerAttribute::valueChanged, this, &VirtualFixtureBlock::updateListener);
    connect(&m_numChannels, &IntegerAttribute::valueChanged, this, &VirtualFixtureBlock::updateNodeCount);
}

NodeBase* VirtualFixtureBlock::getChannelNode(int index) {
    if (index < 0 || index >= m_channelNodes.size()) return nullptr;
    return m_channelNodes[index];
}

void VirtualFixtureBlock::onLevelsChanged() {
    int level = m_listener->mergedLevels()[m_channel - 1].level;
    setValue(limit(0.0, level / 255.0, 1.0));

    for (int i=1; i<m_numChannels; ++i) {
        int ch = m_channel + i;
        if (ch > 512 || (i-1) >= m_channelNodes.size()) return;
        level = m_listener->mergedLevels()[ch - 1].level;
        m_channelNodes[i-1]->setValue(limit(0.0, level / 255.0, 1.0));
    }
}

void VirtualFixtureBlock::updateListener() {
    if (!m_listener.isNull()) {
        disconnect(m_listener.data(), &sACNListener::levelsChanged, this, &VirtualFixtureBlock::onLevelsChanged);
        m_listener.clear();
    }
    m_listener = sACNManager::getInstance()->getListener(m_universe);
    connect(m_listener.data(), &sACNListener::levelsChanged, this, &VirtualFixtureBlock::onLevelsChanged);
    onLevelsChanged();
}

void VirtualFixtureBlock::updateNodeCount() {
    int diff = m_numChannels - 1 - m_channelNodes.size();
    if (diff < 0) {
        for (int i=0; i < -diff; ++i) {
            NodeBase* node = m_channelNodes.last();
            m_channelNodes.pop_back();
            removeNode(node);
        }
    } else {
        for (int i=0; i<diff; ++i) {
            NodeBase* node = createOutputNode(QString::number(m_channelNodes.size()));
            m_channelNodes.append(node);
        }
    }
    m_actualChannelCount = m_numChannels.getValue();
}
