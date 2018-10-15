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

#ifndef ONEINPUTBLOCK_H
#define ONEINPUTBLOCK_H

#include "BlockBase.h"
#include <QPointer>

// forward declaration to reduce dependencies
class NodeBase;

/**
 * @brief The OneInputBlock class is the base of all simple blocks with one input node.
 */
class OneInputBlock : public BlockBase {

    Q_OBJECT

public:

	/**
	 * @brief OneInputBlock creates a Block instance with one input node
	 * @param controller a pointer to the main controller
     * @param uid of this block or nothing if this block is new
	 */
    explicit OneInputBlock(MainController* controller, QString uid);

protected:
	/**
	 * @brief m_inputNode is a pointer to the created input node
	 */
    QPointer<NodeBase> m_inputNode;

};


#endif // ONEINPUTBLOCK_H
