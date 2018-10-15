#ifndef HOG4FADERBLOCK_H
#define HOG4FADERBLOCK_H

#include "core/block_data/OneInputBlock.h"
#include "osc/OSCMessage.h"
#include "utils.h"


class Hog4FaderBlock: public OneInputBlock {

    Q_OBJECT

public:

    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Hog4 Master Fader";
        info.nameInUi = "Playback Master";
        info.category << "ETC Consoles" << "Hog 4";
        info.helpText = "Controls a master fader of the connected console.\n\n"
                        "Note: Controlling faders is not possible when using Hog 4 PC "
                        "without connected hardware!";
        info.qmlFile = "qrc:/qml/Blocks/Hog4/Hog4FaderBlock.qml";
        info.complete<Hog4FaderBlock>();
        return info;
    }

    explicit Hog4FaderBlock(MainController* controller, QString uid);

    // block needs to be rendered for MIDI mapping to work
    virtual bool renderIfNotVisible() const override { return true; }

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

protected slots:
    void onMessageReceived(OSCMessage msg);

    void sendChooseButton();
    void sendGoButton();
    void sendPauseButton();
    void sendBackButton();
    void sendFlashButton();
    void sendFaderValue();

protected:
    QPointer<NodeBase> m_chooseNode;
    QPointer<NodeBase> m_goNode;
    QPointer<NodeBase> m_pauseNode;
    QPointer<NodeBase> m_backNode;
    QPointer<NodeBase> m_flashNode;

    IntegerAttribute m_masterNumber;
    BoolAttribute m_chooseButton;
    BoolAttribute m_goButton;
    BoolAttribute m_pauseButton;
    BoolAttribute m_backButton;
    BoolAttribute m_flashButton;
    DoubleAttribute m_faderValue;

    BoolAttribute m_chooseLed;
    BoolAttribute m_flashLed;

    int m_lastSentFaderValue;
};

#endif // HOG4FADERBLOCK_H
