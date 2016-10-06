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

#include "BlockList.h"

#include "block_implementations/DimmerBlock.h"
#include "block_implementations/SliderBlock.h"
#include "block_implementations/SwitchBlock.h"
#include "block_implementations/ButtonBlock.h"
#include "block_implementations/ScriptBlock.h"
#include "block_implementations/ControlsTestBlock.h"
#include "block_implementations/ColorwheelBlock.h"
#include "block_implementations/MidiNoteInBlock.h"
#include "block_implementations/MidiNoteInRangeBlock.h"
#include "block_implementations/MidiNoteOutBlock.h"
#include "block_implementations/MidiControlInBlock.h"
#include "block_implementations/MidiControlOutBlock.h"
#include "block_implementations/MidiProgramInBlock.h"
#include "block_implementations/MidiProgramOutBlock.h"
#include "block_implementations/MidiMonitorBlock.h"
#include "block_implementations/EosChanBlock.h"
#include "block_implementations/EosSubBlock.h"
#include "block_implementations/EosGroupBlock.h"
#include "block_implementations/EosCueBlock.h"
#include "block_implementations/EosKeyBlock.h"
#include "block_implementations/EosMacroBlock.h"
#include "block_implementations/EosCmdBlock.h"
#include "block_implementations/EosInfoBlock.h"
#include "block_implementations/EosCueListBlock.h"
#include "block_implementations/EosSingleFaderBlock.h"
#include "block_implementations/EosFaderBankBlock.h"
#include "block_implementations/EosMasterPlaybackBlock.h"
#include "block_implementations/EosWheelsBlock.h"
#include "block_implementations/EosOscMonitorBlock.h"
#include "block_implementations/EosEffectBlock.h"
#include "block_implementations/OscOutBlock.h"
#include "block_implementations/OscInBlock.h"
#include "block_implementations/OscMonitorBlock.h"
#include "block_implementations/DelayBlock.h"
#include "block_implementations/ThresholdBlock.h"
#include "block_implementations/DecayBlock.h"
#include "block_implementations/HoldMaxBlock.h"
#include "block_implementations/BpmBlock.h"
#include "block_implementations/RandomValueBlock.h"
#include "block_implementations/DebugBlock.h"
#include "block_implementations/NotesBlock.h"
#include "block_implementations/PreviewBlock.h"
#include "block_implementations/LogBlock.h"
#include "block_implementations/LuminosusInfoBlock.h"
#include "block_implementations/StopwatchBlock.h"
#include "block_implementations/QmlOnlyBlock.h"
#include "block_implementations/AudioPlaybackBlock.h"
#include "block_implementations/ClockBlock.h"
#include "block_implementations/LinearValueBlock.h"
#include "block_implementations/SinusValueBlock.h"
#include "block_implementations/TickGeneratorBlock.h"
#include "block_implementations/AudioLevelBlock.h"
#include "block_implementations/BeatDetectionBlock.h"
#include "block_implementations/AudioVolumeBlock.h"
#include "block_implementations/PageAnchorBlock.h"
#include "block_implementations/StyledTextBlock.h"
#include "block_implementations/ImageBlock.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>


BlockList::BlockList()
	: QObject(0)
{
	checkAvailableDependencies();
	addAllBlocks();
}

bool BlockList::blockExists(QString name) const {
	return mapContains(m_blockNames, name);
}

const BlockInfo& BlockList::getBlockInfoByName(QString name) const {
	auto it = m_blockNames.find(name);
	if (it == m_blockNames.end()) {
		qCritical() << "Could not find block info for block type: " << name;
        // TODO: replace with dummy stack variable?
		return *(new BlockInfo());
	}
	return it->second;
}

QString BlockList::getJsonBlockModel() const {
	// available blocks do not change at runtime
	// -> create model only if it doesn't already exist:
	if (m_jsonBlockModel.isEmpty()) {
		QJsonArray model;
        for (const BlockInfo& info: m_orderedBlockList) {
			QJsonObject jsonInfo;
			jsonInfo["name"] = info.typeName;
			jsonInfo["nameInUi"] = info.nameInUi;
			jsonInfo["category"] = QJsonArray::fromStringList(info.category);
			model.append(jsonInfo);
		}
		m_jsonBlockModel = QJsonDocument(model).toJson();
	}
	return m_jsonBlockModel;
}

QString BlockList::getSearchResult(QString query) const {
	QString queryLow = query.toLower();
	QJsonArray result;
    for (const BlockInfo& info: m_orderedBlockList) {
		// check for query:
		if (blockMatchesQuery(info, queryLow)) {
			// block matches query -> add it to result:
			QJsonObject jsonInfo;
			jsonInfo["name"] = info.typeName;
			jsonInfo["nameInUi"] = info.nameInUi;
			jsonInfo["category"] = QJsonArray::fromStringList(info.category);
			result.append(jsonInfo);
		}
	}
	return QJsonDocument(result).toJson();
}

QStringList BlockList::getAllBlockTypes() const {
	QStringList blockTypes;
	for (auto entry: m_blockNames) {
		blockTypes.append(entry.first);
	}
	return blockTypes;
}

void BlockList::checkAvailableDependencies() {
    m_availableDependencies.insert(BlockDependency::Experimental);
    m_availableDependencies.insert(BlockDependency::NonETC);
    m_availableDependencies.insert(BlockDependency::Debugging);
    m_availableDependencies.insert(BlockDependency::AudioInput);
#ifdef RT_MIDI_AVAILABLE
	m_availableDependencies.insert(BlockDependency::Midi);
#endif
}

void BlockList::addAllBlocks() {
    // Controls
    addBlock(SliderBlock::info());
    addBlock(ButtonBlock::info());
    addBlock(SwitchBlock::info());
    addBlock(ColorwheelBlock::info());

    // Logic
    addBlock(ThresholdBlock::info());
    addBlock(DelayBlock::info());
    addBlock(DecayBlock::info());
    addBlock(HoldMaxBlock::info());
    addBlock(ScriptBlock::info());
    addBlock(BpmBlock::info());

    // Generator
    addBlock(LinearValueBlock::info());
    addBlock(SinusValueBlock::info());
    addBlock(RandomValueBlock::info());
    addBlock(TickGeneratorBlock::info());

    // Eos
    addBlock(EosChanBlock::info());
    addBlock(EosGroupBlock::info());
    addBlock(EosSubBlock::info());
    addBlock(EosKeyBlock::info());
    addBlock(EosMacroBlock::info());
    addBlock(EosEffectBlock::info());
    addBlock(EosCueBlock::info());
    addBlock(EosCueListBlock::info());
    addBlock(EosSingleFaderBlock::info());
    addBlock(EosFaderBankBlock::info());
    addBlock(EosMasterPlaybackBlock::info());
    addBlock(EosWheelsBlock::info());
    addBlock(EosCmdBlock::info());
    addBlock(EosInfoBlock::info());
    addBlock(EosOscMonitorBlock::info());

    // OSC
    addBlock(OscInBlock::info());
    addBlock(OscOutBlock::info());
    addBlock(OscMonitorBlock::info());

    // MIDI
    addBlock(MidiNoteInBlock::info());
    addBlock(MidiNoteInRangeBlock::info());
    addBlock(MidiNoteOutBlock::info());
    addBlock(MidiControlInBlock::info());
    addBlock(MidiControlOutBlock::info());
    addBlock(MidiProgramInBlock::info());
    addBlock(MidiProgramOutBlock::info());
    addBlock(MidiMonitorBlock::info());

    // Media Playback
    addBlock(AudioPlaybackBlock::info());

    // Sound2Light
    addBlock(AudioLevelBlock::info());
    addBlock(BeatDetectionBlock::info());
    addBlock(AudioVolumeBlock::info());

    // Presentation
    addBlock(PageAnchorBlock::info());
    addBlock(StyledTextBlock::info());
    addBlock(ImageBlock::info());

	// Other
	addBlock(NotesBlock::info());
    addBlock(ClockBlock::info());
    addBlock(PreviewBlock::info());
    addBlock(StopwatchBlock::info());
    addBlock(DimmerBlock::info());
    addBlock(QmlOnlyBlock::info());

    // Debugging:
    addBlock(LogBlock::info());
    addBlock(DebugBlock::info());
    addBlock(ControlsTestBlock::info());
    addBlock(LuminosusInfoBlock::info());
}

void BlockList::addBlock(const BlockInfo& info) {
	// check dependencies:
	for (BlockDependency dependency: info.dependencies) {
		if (!m_availableDependencies.contains(dependency)) {
			// dependency is not available
			qDebug() << "Missing dependencies for block: " << info.typeName;
			return;
		}
	}
	m_blockNames[info.typeName] = info;
    m_orderedBlockList.append(info);
}

bool BlockList::blockMatchesQuery(const BlockInfo& info, const QString& query) const {
	// depending on length of query:
	if (query.size() < 3) {
		// if query is shorter than 3 characters, look only for beginning of names:
		if (info.typeName.toLower().startsWith(query)) {
			return true;
		} else if (info.nameInUi.toLower().startsWith(query)) {
			return true;
		} else if (!info.category.isEmpty() && info.category[0].toLower().startsWith(query)) {
			return true;
		}
	} else {
		// if query is 3 chars or longer, checks the whole strings for a match:
		if (info.typeName.toLower().contains(query)) {
			return true;
		} else if (info.nameInUi.toLower().contains(query)) {
			return true;
		} else if (!info.category.isEmpty() && info.category[0].toLower().contains(query)) {
			return true;
		}
	}
	return false;
}
