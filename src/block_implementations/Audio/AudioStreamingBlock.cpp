#include "AudioStreamingBlock.h"

#include "core/MainController.h"


AudioStreamingBlock::AudioStreamingBlock(MainController* controller, QString uid)
    : OneInputBlock(controller, uid)
    , m_stopNode(nullptr)
    , m_url(this, "url")
{
    m_stopNode = createInputNode("stop");
    m_inputNode->enableImpulseDetection();
    connect(m_inputNode, SIGNAL(impulseBegin()), this, SLOT(play()));
    m_stopNode->enableImpulseDetection();
    connect(m_stopNode, SIGNAL(impulseBegin()), this, SLOT(stop()));
    m_stopNode->setActive(false);
}

void AudioStreamingBlock::play() {
    if (m_url.getValue().isEmpty()) return;
    m_player.setMedia(QUrl(m_url));
    m_player.play();
    m_stopNode->setActive(true);
}

void AudioStreamingBlock::stop() {
    m_player.stop();
    m_stopNode->setActive(false);
}
