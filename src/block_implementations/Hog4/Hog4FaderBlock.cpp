#include "Hog4FaderBlock.h"

#include "core/MainController.h"
#include "core/Nodes.h"


Hog4FaderBlock::Hog4FaderBlock(MainController* controller, QString uid)
    : OneInputBlock(controller, uid)
    , m_chooseNode(nullptr)
    , m_goNode(nullptr)
    , m_pauseNode(nullptr)
    , m_backNode(nullptr)
    , m_flashNode(nullptr)
    , m_masterNumber(this, "masterNumber", 1, 1, 9999)
    , m_chooseButton(this, "chooseButton", false, /*persistent*/ false)
    , m_goButton(this, "goButton", false, /*persistent*/ false)
    , m_pauseButton(this, "pauseButton", false, /*persistent*/ false)
    , m_backButton(this, "backButton", false, /*persistent*/ false)
    , m_flashButton(this, "flashButton", false, /*persistent*/ false)
    , m_faderValue(this, "faderValue", 0, 0, 1, /*persistent*/ false)
    , m_chooseLed(this, "chooseLed", false, /*persistent*/ false)
    , m_flashLed(this, "flashLed", false, /*persistent*/ false)
    , m_lastSentFaderValue(0)
{
    m_chooseNode = createInputNode("choose");
    m_goNode = createInputNode("go");
    m_pauseNode = createInputNode("pause");
    m_backNode = createInputNode("back");
    m_flashNode = createInputNode("flash");

    m_chooseNode->enableImpulseDetection();
    connect(m_chooseNode, &NodeBase::impulseBegin, [this](){ this->m_chooseButton = true; });
    connect(m_chooseNode, &NodeBase::impulseEnd, [this](){ this->m_chooseButton = false; });
    m_goNode->enableImpulseDetection();
    connect(m_goNode, &NodeBase::impulseBegin, [this](){ this->m_goButton = true; });
    connect(m_goNode, &NodeBase::impulseEnd, [this](){ this->m_goButton = false; });
    m_pauseNode->enableImpulseDetection();
    connect(m_pauseNode, &NodeBase::impulseBegin, [this](){ this->m_pauseButton = true; });
    connect(m_pauseNode, &NodeBase::impulseEnd, [this](){ this->m_pauseButton = false; });
    m_backNode->enableImpulseDetection();
    connect(m_backNode, &NodeBase::impulseBegin, [this](){ this->m_backButton = true; });
    connect(m_backNode, &NodeBase::impulseEnd, [this](){ this->m_backButton = false; });
    m_flashNode->enableImpulseDetection();
    connect(m_flashNode, &NodeBase::impulseBegin, [this](){ this->m_flashButton = true; });
    connect(m_flashNode, &NodeBase::impulseEnd, [this](){ this->m_flashButton = false; });

    connect(m_inputNode, &NodeBase::dataChanged, [this](){ this->m_faderValue = m_inputNode->getValue(); });

    connect(&m_masterNumber, &IntegerAttribute::valueChanged, [this](){ m_chooseLed = false; });
    connect(&m_chooseButton, &BoolAttribute::valueChanged, this, &Hog4FaderBlock::sendChooseButton);
    connect(&m_goButton, &BoolAttribute::valueChanged, this, &Hog4FaderBlock::sendGoButton);
    connect(&m_pauseButton, &BoolAttribute::valueChanged, this, &Hog4FaderBlock::sendPauseButton);
    connect(&m_backButton, &BoolAttribute::valueChanged, this, &Hog4FaderBlock::sendBackButton);
    connect(&m_flashButton, &BoolAttribute::valueChanged, this, &Hog4FaderBlock::sendFlashButton);
    connect(&m_faderValue, &DoubleAttribute::valueChanged, this, &Hog4FaderBlock::sendFaderValue);

    connect(controller->lightingConsole(), SIGNAL(messageReceived(OSCMessage)), this, SLOT(onMessageReceived(OSCMessage)));
}

void Hog4FaderBlock::onMessageReceived(OSCMessage msg) {
    if (msg.pathStartsWith("/hog/status/led/")
            && msg.pathPart(4) == QString::number(m_masterNumber)) {
        if (msg.pathPart(3) == "choose") {
            m_chooseLed = msg.arguments().first().toInt() == 1;
        } else if (msg.pathPart(3) == "flash") {
            m_flashLed = msg.arguments().first().toInt() == 1;
        }
    } else if (msg.pathStartsWith("/hog/hardware/fader/")
               && msg.pathPart(3) == QString::number(m_masterNumber)) {
        m_lastSentFaderValue = msg.arguments().first().toInt();
        m_faderValue = msg.arguments().first().toInt() / 255.0;
    }
}

void Hog4FaderBlock::sendChooseButton() {
    QString message = "/hog/hardware/choose/%1";
    message = message.arg(QString::number(m_masterNumber));
    m_controller->lightingConsole()->sendMessage(message, m_chooseButton ? 1.0 : 0.0);
}

void Hog4FaderBlock::sendGoButton() {
    QString message = "/hog/hardware/go/%1";
    message = message.arg(QString::number(m_masterNumber));
    m_controller->lightingConsole()->sendMessage(message, m_goButton ? 1.0 : 0.0);
}

void Hog4FaderBlock::sendPauseButton() {
    QString message = "/hog/hardware/pause/%1";
    message = message.arg(QString::number(m_masterNumber));
    m_controller->lightingConsole()->sendMessage(message, m_pauseButton ? 1.0 : 0.0);
}

void Hog4FaderBlock::sendBackButton() {
    QString message = "/hog/hardware/goback/%1";
    message = message.arg(QString::number(m_masterNumber));
    m_controller->lightingConsole()->sendMessage(message, m_backButton ? 1.0 : 0.0);
}

void Hog4FaderBlock::sendFlashButton() {
    QString message = "/hog/hardware/flash/%1";
    message = message.arg(QString::number(m_masterNumber));
    m_controller->lightingConsole()->sendMessage(message, m_flashButton ? 1.0 : 0.0);
}

void Hog4FaderBlock::sendFaderValue() {
    int faderValue = int(m_faderValue * 255);
    if (faderValue == m_lastSentFaderValue) return;
    QString message = "/hog/hardware/fader/%1";
    message = message.arg(QString::number(m_masterNumber));
    m_controller->lightingConsole()->sendMessage(message, double(faderValue));
    m_lastSentFaderValue = faderValue;
}
