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

#ifndef COLORWHEELBLOCK_H
#define COLORWHEELBLOCK_H

#include "block_data/BlockBase.h"
#include "NodeBase.h"
#include "block_data/OneOutputBlock.h"

class ColorwheelBlock : public OneOutputBlock
{
    Q_OBJECT

public:

	static BlockInfo info() {
		static BlockInfo info;
		info.name = "Colorwheel";
		info.category << "Other";
		info.dependencies = {BlockDependency::Experimental};
		info.qmlFile = "qrc:/qml/Blocks/ColorwheelBlock.qml";
		info.complete<ColorwheelBlock>();
		return info;
	}

	explicit ColorwheelBlock(MainController* controller, QString uid) : OneOutputBlock(controller, uid, info().qmlFile) {

	}

public slots:
	virtual BlockInfo getBlockInfo() const override { return info(); }
};

#endif // COLORWHEELBLOCK_H
