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

#ifndef GLOBALOSCCOMMANDS_H
#define GLOBALOSCCOMMANDS_H

#include "OSCMessage.h"

#include <QObject>

// Forward declaration to reduce dependencies
class MainController;


namespace GlobalOscCommandsConstants {

/**
 * @brief pathPrefix is the start of all global command OSC messages for this software
 * @memberof GlobalOscCommands
 */
static const QString pathPrefix = "/lumi/";

/**
 * @brief projectChange is the second part of the path of a message to change the project
 * @memberof GlobalOscCommands
 */
static const QString projectChange = "project";

}

/**
 * @brief The GlobalOscCommands class maps incoming OSC messages to global functions.
 */
class GlobalOscCommands : public QObject
{
	Q_OBJECT

public:
	/**
	 * @brief creates an instance and connects the OSC message received signal
	 * with the handleMessage slot
	 * @param controller a pointer to the main controller
	 */
	explicit GlobalOscCommands(MainController* controller);

public slots:

	/**
	 * @brief maps incoming messages to global functions (i.e. project change)
	 * @param msg the incoming message
	 */
	void handleMessage(OSCMessage msg);

protected:
	/**
	 * @brief m_controller pointer to MainController instance
	 */
	MainController* m_controller;
};

#endif // GLOBALOSCCOMMANDS_H
