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

#ifndef MIDIMONITORBLOCK_H
#define MIDIMONITORBLOCK_H

#include "core/block_data/BlockBase.h"


class MidiMonitorBlock : public BlockBase
{
	Q_OBJECT

public:

	static BlockInfo info() {
		static BlockInfo info;
		info.typeName = "MIDI Monitor";
        info.nameInUi = "MIDI Monitor";
        info.category << "MIDI";
		info.availabilityRequirements = {AvailabilityRequirement::Midi};
		info.helpText = "With the buttons at the top you can choose if incoming and / or outgoing "
						"messages will be logged and displayed.";
        info.qmlFile = "qrc:/qml/Blocks/MIDI/MidiMonitorBlock.qml";
		info.complete<MidiMonitorBlock>();
		return info;
	}

    explicit MidiMonitorBlock(MainController* controller, QString uid);

public slots:
	virtual BlockInfo getBlockInfo() const override { return info(); }
};

#endif // MIDIMONITORBLOCK_H
