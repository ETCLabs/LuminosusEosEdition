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

#ifndef MIDINOTEINBLOCK_H
#define MIDINOTEINBLOCK_H

#include "core/block_data/OneOutputBlock.h"
#include "midi/MidiManager.h"


class MidiNoteInBlock : public OneOutputBlock
{
    Q_OBJECT

	Q_PROPERTY(int key READ getKey WRITE setKey NOTIFY keyChanged)
	Q_PROPERTY(int channel READ getChannel WRITE setChannel NOTIFY channelChanged)
	Q_PROPERTY(bool useDefaultChannel READ getUseDefaultChannel WRITE setUseDefaultChannel NOTIFY useDefaultChannelChanged)
	Q_PROPERTY(int tone READ getTone WRITE setTone NOTIFY keyChanged)
	Q_PROPERTY(int octave READ getOctave WRITE setOctave NOTIFY keyChanged)
    Q_PROPERTY(bool learning READ getLearning NOTIFY learningChanged)

public:

	static BlockInfo info() {
		static BlockInfo info;
		info.typeName = "MIDI Note In";
		info.nameInUi = "Note In";
        info.category << "MIDI";
		info.availabilityRequirements = {AvailabilityRequirement::Midi};
		info.helpText = "Outputs the velocity of incoming messages for the selected note.\n"
						"Note Off messages will be interpreted as Note On with velocity 0.\n"
						"The middle C is here C5.";
        info.qmlFile = "qrc:/qml/Blocks/MIDI/MidiNoteInBlock.qml";
		info.complete<MidiNoteInBlock>();
		return info;
	}

	explicit MidiNoteInBlock(MainController* controller, QString uid);

	virtual void getAdditionalState(QJsonObject& state) const override;
	virtual void setAdditionalState(const QJsonObject& state) override;

signals:
	void keyChanged();
	void channelChanged();
	void validMessageReceived();
	void useDefaultChannelChanged();
    void learningChanged();

public slots:
	virtual BlockInfo getBlockInfo() const override { return info(); }

	void onMidiMessage(MidiEvent event);

    void startLearning();
    void checkIfEventFits(MidiEvent event);

    // ------------------------- Getter + Setter -----------------------------

	int getKey() const { return m_key; }
	void setKey(int value) { m_key = limit(0, value, 127); emit keyChanged(); }

	int getChannel() const { return m_channel; }
	void setChannel(int value) { m_channel = limit(1, value, 16); emit channelChanged(); }

	bool getUseDefaultChannel() const { return m_useDefaultChannel; }
	void setUseDefaultChannel(bool value) { m_useDefaultChannel = value; emit useDefaultChannelChanged(); }

	int getTone() const { return m_key % 12; }
	void setTone(int value);

	int getOctave() const { return m_key / 12; }
	void setOctave(int value);

    bool getLearning() const { return m_learning; }
    void setLearning(bool value) { m_learning = value; emit learningChanged(); }

protected:
	int m_key;
	int m_channel;
	bool m_useDefaultChannel;
    bool m_learning;
};


#endif // MIDINOTEINBLOCK_H
