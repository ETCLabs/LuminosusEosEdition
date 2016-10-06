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

#ifndef MIDINOTEINRANGEBLOCK_H
#define MIDINOTEINRANGEBLOCK_H

#include "block_data/OneOutputBlock.h"
#include "MidiManager.h"


class MidiNoteInRangeBlock : public OneOutputBlock
{
	Q_OBJECT

	Q_PROPERTY(int key READ getKey WRITE setKey NOTIFY keyChanged)
	Q_PROPERTY(int tone READ getTone WRITE setTone NOTIFY keyChanged)
	Q_PROPERTY(int octave READ getOctave WRITE setOctave NOTIFY keyChanged)

	Q_PROPERTY(int key2 READ getKey2 WRITE setKey2 NOTIFY key2Changed)
	Q_PROPERTY(int tone2 READ getTone2 WRITE setTone2 NOTIFY key2Changed)
	Q_PROPERTY(int octave2 READ getOctave2 WRITE setOctave2 NOTIFY key2Changed)

	Q_PROPERTY(int channel READ getChannel WRITE setChannel NOTIFY channelChanged)
	Q_PROPERTY(bool useDefaultChannel READ getUseDefaultChannel WRITE setUseDefaultChannel NOTIFY useDefaultChannelChanged)

    Q_PROPERTY(bool learning READ getLearning NOTIFY learningChanged)
    Q_PROPERTY(bool learning2 READ getLearning2 NOTIFY learning2Changed)

public:

	static BlockInfo info() {
		static BlockInfo info;
		info.typeName = "MIDI Note In Range";
		info.nameInUi = "Note In (Range)";
        info.category << "Midi";
		info.dependencies = {BlockDependency::Midi};
		info.helpText = "Outputs the velocity of incoming messages in the selected range.\n"
						"Note Off messages will be interpreted as Note On with velocity 0.\n"
						"The most recent value is used (LTP).";
		info.qmlFile = "qrc:/qml/Blocks/MidiNoteInRangeBlock.qml";
		info.complete<MidiNoteInRangeBlock>();
		return info;
	}

	explicit MidiNoteInRangeBlock(MainController* controller, QString uid);

	virtual QJsonObject getState() const override;
	virtual void setState(const QJsonObject& state) override;

signals:
	void keyChanged();
	void key2Changed();
	void channelChanged();
	void useDefaultChannelChanged();
	void validMessageReceived();
    void learningChanged();
    void learning2Changed();

public slots:
	virtual BlockInfo getBlockInfo() const override { return info(); }

	void onMidiMessage(MidiEvent event);

    void startLearning();
    void checkIfEventFits(MidiEvent event);

    void startLearning2();
    void checkIfEventFits2(MidiEvent event);

    // ------------------------- Getter + Setter -----------------------------

	int getKey() const { return m_key; }
	void setKey(int value);

	int getTone() const { return m_key % 12; }
	void setTone(int value);

	int getOctave() const { return m_key / 12; }
	void setOctave(int value);

	int getKey2() const { return m_key2; }
	void setKey2(int value);

	int getTone2() const { return m_key2 % 12; }
	void setTone2(int value);

	int getOctave2() const { return m_key2 / 12; }
	void setOctave2(int value);

	int getChannel() const { return m_channel; }
	void setChannel(int value) { m_channel = limit(1, value, 16); emit channelChanged(); }

	bool getUseDefaultChannel() const { return m_useDefaultChannel; }
	void setUseDefaultChannel(bool value) { m_useDefaultChannel = value; emit useDefaultChannelChanged(); }

    bool getLearning() const { return m_learning; }
    void setLearning(bool value) { m_learning = value; emit learningChanged(); }

    bool getLearning2() const { return m_learning2; }
    void setLearning2(bool value) { m_learning2 = value; emit learning2Changed(); }

protected:
	int m_key;
	int m_key2;
	int m_channel;
	bool m_useDefaultChannel;
    bool m_learning;
    bool m_learning2;
};

#endif // MIDINOTEINRANGEBLOCK_H
