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
#include "block_implementations/AudioLevelBlock.h"
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
#include "block_implementations/EosCueBlock.h"
#include "block_implementations/EosKeyBlock.h"
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

const BlockInfo& BlockList::getBlockInfoByName(QString name) {
	return m_blockNames[name];
}

QString BlockList::getJsonBlockModel() const {
	// available blocks do not change at runtime
	// -> create model only if it doesn't already exist:
	if (m_jsonBlockModel.isEmpty()) {
		QJsonArray model;
		for (auto entry: m_blockNames) {
			const BlockInfo& info = entry.second;
			QJsonObject jsonInfo;
			jsonInfo["name"] = info.name;
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
	for (auto entry: m_blockNames) {
		const BlockInfo& info = entry.second;

		// check for query:
		if (blockMatchesQuery(info, queryLow)) {
			// block matches query -> add it to result:
			QJsonObject jsonInfo;
			jsonInfo["name"] = info.name;
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
	m_availableDependencies.insert(BlockDependency::AudioInput);
	//m_availableDependencies.insert(BlockDependency::Experimental);
	//m_availableDependencies.insert(BlockDependency::NonETC);
#ifdef RT_MIDI_AVAILABLE
	m_availableDependencies.insert(BlockDependency::Midi);
#endif
}

void BlockList::addAllBlocks() {
	addBlock(DimmerBlock::info());
	addBlock(DebugBlock::info());
	addBlock(NotesBlock::info());

	// Controls
	addBlock(SliderBlock::info());
	addBlock(SwitchBlock::info());
	addBlock(ButtonBlock::info());
	addBlock(ColorwheelBlock::info());
	addBlock(ControlsTestBlock::info());

	// Logic
	addBlock(ScriptBlock::info());
	addBlock(DelayBlock::info());
	addBlock(ThresholdBlock::info());
	addBlock(DecayBlock::info());
	addBlock(HoldMaxBlock::info());
	addBlock(BpmBlock::info());

	// MIDI
	addBlock(MidiNoteInBlock::info());
	addBlock(MidiNoteInRangeBlock::info());
	addBlock(MidiNoteOutBlock::info());
	addBlock(MidiControlInBlock::info());
	addBlock(MidiControlOutBlock::info());
	addBlock(MidiProgramInBlock::info());
	addBlock(MidiProgramOutBlock::info());
	addBlock(MidiMonitorBlock::info());

	// OSC
	addBlock(OscInBlock::info());
	addBlock(OscOutBlock::info());
	addBlock(OscMonitorBlock::info());

	// Eos
	addBlock(EosChanBlock::info());
	addBlock(EosCueBlock::info());
	addBlock(EosKeyBlock::info());

	// Sound2Light
	addBlock(AudioLevelBlock::info());

	// Generator
	addBlock(RandomValueBlock::info());
}

void BlockList::addBlock(const BlockInfo& info) {
	// check dependencies:
	for (BlockDependency dependency: info.dependencies) {
		if (!m_availableDependencies.contains(dependency)) {
			// dependency is not available
			qDebug() << "Missing dependencies for block: " << info.name;
			return;
		}
	}
	m_blockNames[info.name] = info;
}

bool BlockList::blockMatchesQuery(const BlockInfo& info, const QString& query) const {
	// depending on length of query:
	if (query.size() < 3) {
		// if query is shorter than 3 characters, look only for beginning of names:
		if (info.name.toLower().startsWith(query)) {
			return true;
		} else if (info.nameInUi.toLower().startsWith(query)) {
			return true;
		} else if (!info.category.isEmpty() && info.category[0].toLower().startsWith(query)) {
			return true;
		}
	} else {
		// if query is 3 chars or longer, checks the whole strings for a match:
		if (info.name.toLower().contains(query)) {
			return true;
		} else if (info.nameInUi.toLower().contains(query)) {
			return true;
		} else if (!info.category.isEmpty() && info.category[0].toLower().contains(query)) {
			return true;
		}
	}
	return false;
}
