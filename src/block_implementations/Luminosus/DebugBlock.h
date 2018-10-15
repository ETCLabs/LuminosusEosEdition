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

#ifndef DEBUGBLOCK_H
#define DEBUGBLOCK_H

#include "core/block_data/BlockBase.h"


class DebugBlock : public BlockBase
{
	Q_OBJECT

public:

	static BlockInfo info() {
		static BlockInfo info;
		info.typeName = "Debug";
        info.nameInUi = "Block Management";
        info.category << "General" << "Debugging";
        info.visibilityRequirements << VisibilityRequirement::DeveloperMode;
        info.helpText = "Exists to quickly test if all Blocks work and the program doesn't crash.\n"
                        "Can also be used to do stress tests with many Blocks.";
        info.qmlFile = "qrc:/qml/Blocks/Luminosus/DebugBlock.qml";
		info.complete<DebugBlock>();
		return info;
	}

	explicit DebugBlock(MainController* controller, QString uid)
		: BlockBase(controller, uid)
	{

	}

public slots:
	virtual BlockInfo getBlockInfo() const override { return info(); }

	void addAllBlockTypes();

	void deleteAllBlocks();
};

#endif // DEBUGBLOCK_H
