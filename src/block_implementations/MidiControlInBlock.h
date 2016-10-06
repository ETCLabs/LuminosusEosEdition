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

#ifndef MIDICONTROLINBLOCK_H
#define MIDICONTROLINBLOCK_H

#include "block_data/OneOutputBlock.h"
#include "MidiManager.h"


class MidiControlInBlock : public OneOutputBlock
{
	Q_OBJECT

	Q_PROPERTY(int target READ getTarget WRITE setTarget NOTIFY targetChanged)
	Q_PROPERTY(int channel READ getChannel WRITE setChannel NOTIFY channelChanged)
	Q_PROPERTY(bool useDefaultChannel READ getUseDefaultChannel WRITE setUseDefaultChannel NOTIFY useDefaultChannelChanged)
    Q_PROPERTY(bool learning READ getLearning NOTIFY learningChanged)

public:

	static BlockInfo info() {
		static BlockInfo info;
		info.typeName = "MIDI Control In";
		info.nameInUi = "Control Change In";
        info.category << "Midi";
		info.dependencies = {BlockDependency::Midi};
		info.helpText = "Outputs the value of matching incoming MIDI Control Change messages.";
		info.qmlFile = "qrc:/qml/Blocks/MidiControlInBlock.qml";
		info.complete<MidiControlInBlock>();
		return info;
	}

	explicit MidiControlInBlock(MainController* controller, QString uid);

	virtual QJsonObject getState() const override;
	virtual void setState(const QJsonObject& state) override;

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
};

#endif // MIDICONTROLINBLOCK_H
