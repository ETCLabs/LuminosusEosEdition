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

#ifndef LAUNCHPADBUTTONBLOCK
#define LAUNCHPADBUTTONBLOCK

#include "core/block_data/OneOutputBlock.h"
#include "core/SmartAttribute.h"


class LaunchpadButtonBlock : public OneOutputBlock
{
    Q_OBJECT

public:

	static BlockInfo info() {
		static BlockInfo info;
        info.typeName = "LaunchpadButton";
        info.category << "General" << "Controls";
        info.helpText = "A simple launchpad button.\n"
                        "Outputs 1 if pressed and 0 if not.\n"
                        "User can set custom values for MIDI feedback\n"
                        "(e. g. for using Launchpad colors)";
        info.qmlFile = "qrc:/qml/Blocks/Controls/LaunchpadButtonBlock.qml";
		info.complete<LaunchpadButtonBlock>();
		return info;
	}

    explicit LaunchpadButtonBlock(MainController* controller, QString uid);

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

protected:
    BoolAttribute m_toggleMode;
    StringAttribute m_labelText;
    IntegerAttribute m_lowValue;
    IntegerAttribute m_highValue;
};

#endif // LAUNCHPADBUTTONBLOCK

