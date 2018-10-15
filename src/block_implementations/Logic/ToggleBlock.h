#ifndef TOGGLEBLOCK_H
#define TOGGLEBLOCK_H

#include "core/block_data/InOutBlock.h"


class ToggleBlock : public InOutBlock
{
    Q_OBJECT

public:
    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Toggle";
        info.nameInUi = "Toggle On-Off";
        info.category << "General" << "Logic";
        info.helpText = "Switches the output between on and of each time a signal is received.\n"
                        "Turns the output on when a signal is received from the 'On' input "
                        "and off correspondingly with the 'Off' input.\n\n"
                        "Useful to create an 'On' and 'Off' button for something or turn a light on"
                        "when Cue A is activated and off when Cue B is activated.";
        info.qmlFile = "qrc:/qml/Blocks/Logic/ToggleBlock.qml";
        info.complete<ToggleBlock>();
        return info;
    }

    explicit ToggleBlock(MainController* controller, QString uid);

    virtual void setAdditionalState(const QJsonObject&) override;

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    void toggle();
    void on();
    void off();

protected:
    QPointer<NodeBase> m_onNode;
    QPointer<NodeBase> m_offNode;

    DoubleAttribute m_value;
};

#endif // TOGGLEBLOCK_H
