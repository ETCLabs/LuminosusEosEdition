#ifndef EOSENCODERBLOCK_H
#define EOSENCODERBLOCK_H

#include "core/block_data/BlockBase.h"
#include "core/SmartAttribute.h"
#include "eos_specific/EosOSCMessage.h"
#include "midi/MidiManager.h"


class EosEncoderBlock : public BlockBase
{
    Q_OBJECT

    Q_PROPERTY(int target READ getTarget WRITE setTarget NOTIFY targetChanged)
    Q_PROPERTY(int channel READ getChannel WRITE setChannel NOTIFY channelChanged)
    Q_PROPERTY(bool useDefaultChannel READ getUseDefaultChannel WRITE setUseDefaultChannel NOTIFY useDefaultChannelChanged)
    Q_PROPERTY(bool learning READ getLearning NOTIFY learningChanged)

public:

    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Eos Encoder";
        info.nameInUi = "Encoder";
        info.category << "ETC Consoles" << "Eos";
        info.availabilityRequirements = {AvailabilityRequirement::Midi};
        info.helpText = "Controls an Eos encoder with a MIDI encoder that sends relative values. "
                        "Use 'Learn' to select the MIDI encoder.<br><br>"
                        "<b>Using with BCF2000:</b><br><br>"
                        "1. Configure encoder on BCF2000 by holding EDIT and turning the encoder, then "
                        "turn encoder 1 (TYPE) till 'CC' appears and "
                        "encoder 6 (MODE) till 'rel1' appears.<br><br>"
                        "2. Enter the name of the parameter to control in the Block "
                        "(it can be 'pan' or 'tilt' or set 'active' option in configuration and "
                        "use a number (1, 2, ...) of active wheel).<br><br>"
                        "3. At last click 'Learn' above and turn the encoder.<br><br>"
                        "4. (optional) Map a MIDI button to the 'Coarse / Fine' button.<br><br><br>"
                        "'feedback' checkbox in configuration can be used to control led ring "
                        "e.g. on Behringer x-touch.<br><br><br>"
                        "This block uses Eos OSC user 1.";
        info.qmlFile = "qrc:/qml/Blocks/Eos/EosEncoderBlock.qml";
        info.complete<EosEncoderBlock>();
        return info;
    }

    explicit EosEncoderBlock(MainController* controller, QString uid);

    virtual void getAdditionalState(QJsonObject& state) const override;
    virtual void setAdditionalState(const QJsonObject& state) override;

    // block needs to be rendered for MIDI mapping to work
    virtual bool renderIfNotVisible() const override { return true; }

signals:
    void targetChanged();
    void channelChanged();
    void useDefaultChannelChanged();
    void validMessageReceived();
    void learningChanged();

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    void onMidiMessage(MidiEvent event);

    void startLearning();
    void checkIfEventFits(MidiEvent event);

    void onIncomingEosMessage(const EosOSCMessage& msg);
    void onFeedbackEnabledChanged();

    // ------------------------- Getter + Setter -----------------------------

    int getTarget() const { return m_target; }
    void setTarget(int value) { m_target = limit(0, value, 119); emit targetChanged(); }

    int getChannel() const { return m_channel; }
    void setChannel(int value) { m_channel = limit(1, value, 16); emit channelChanged(); }

    bool getUseDefaultChannel() const { return m_useDefaultChannel; }
    void setUseDefaultChannel(bool value) { m_useDefaultChannel = value; emit useDefaultChannelChanged(); }

    bool getLearning() const { return m_learning; }
    void setLearning(bool value) { m_learning = value; emit learningChanged(); }

protected:
    int m_target;
    int m_channel;
    bool m_useDefaultChannel;
    bool m_learning;

    StringAttribute m_parameterName;
    BoolAttribute m_fineMode;
    BoolAttribute m_active;
    BoolAttribute m_accelerate;
    BoolAttribute m_feedbackEnabled;
};

#endif // EOSENCODERBLOCK_H
