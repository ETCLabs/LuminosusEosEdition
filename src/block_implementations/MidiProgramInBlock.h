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

#ifndef MIDIPROGRAMINBLOCK_H
#define MIDIPROGRAMINBLOCK_H

#include "block_data/OneOutputBlock.h"
#include "MidiManager.h"


class MidiProgramInBlock : public OneOutputBlock
{
	Q_OBJECT

    Q_PROPERTY(int program READ getProgram WRITE setProgram NOTIFY programChanged)
	Q_PROPERTY(int channel READ getChannel WRITE setChannel NOTIFY channelChanged)
	Q_PROPERTY(bool useDefaultChannel READ getUseDefaultChannel WRITE setUseDefaultChannel NOTIFY useDefaultChannelChanged)
	Q_PROPERTY(bool programIsActive MEMBER m_programIsActive NOTIFY programIsActiveChanged)
    Q_PROPERTY(bool learning READ getLearning NOTIFY learningChanged)

public:

	static BlockInfo info() {
		static BlockInfo info;
		info.typeName = "MIDI Program In";
		info.nameInUi = "Program Change In";
        info.category << "Midi";
		info.dependencies = {BlockDependency::Midi};
		info.helpText = "Sets the output to on if a matching MIDI Program Change message has been received.\n\n"
						"If a Program Change message for a different program is received, the output is set to off.";
		info.qmlFile = "qrc:/qml/Blocks/MidiProgramInBlock.qml";
		info.complete<MidiProgramInBlock>();
		return info;
	}

	explicit MidiProgramInBlock(MainController* controller, QString uid);

	virtual QJsonObject getState() const override;
	virtual void setState(const QJsonObject& state) override;

signals:
    void programChanged();
	void channelChanged();
	void useDefaultChannelChanged();
	void validMessageReceived();
	void programIsActiveChanged();
    void learningChanged();

public slots:
	virtual BlockInfo getBlockInfo() const override { return info(); }

	void onMidiMessage(MidiEvent event);

    void startLearning();
    void checkIfEventFits(MidiEvent event);

    // ------------------------- Getter + Setter -----------------------------

    int getProgram() const { return m_program; }
    void setProgram(int value);

	int getChannel() const { return m_channel; }
	void setChannel(int value) { m_channel = limit(1, value, 16); emit channelChanged(); }

	bool getUseDefaultChannel() const { return m_useDefaultChannel; }
	void setUseDefaultChannel(bool value) { m_useDefaultChannel = value; emit useDefaultChannelChanged(); }

    bool getLearning() const { return m_learning; }
    void setLearning(bool value) { m_learning = value; emit learningChanged(); }

protected:
    int m_program;
	int m_channel;
	bool m_useDefaultChannel;
	bool m_programIsActive;
    bool m_learning;
};

#endif // MIDIPROGRAMINBLOCK_H
