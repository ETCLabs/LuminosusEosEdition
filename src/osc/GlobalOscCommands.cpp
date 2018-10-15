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

#include "GlobalOscCommands.h"

#include "core/MainController.h"

GlobalOscCommands::GlobalOscCommands(MainController* controller)
	: QObject(controller)
	, m_controller(controller)
{
	connect(m_controller->customOsc(), SIGNAL(messageReceived(OSCMessage)),
			this, SLOT(handleMessage(OSCMessage)));
    connect(m_controller->lightingConsole(), SIGNAL(messageReceived(OSCMessage)),
            this, SLOT(handleMessage(OSCMessage)));
}

void GlobalOscCommands::handleMessage(OSCMessage msg) {
	// all global commands start with /lumi:
	if (!msg.pathStartsWith(GlobalOscCommandsConstants::pathPrefix)) {
		return;
	}

	if (msg.pathPart(1) == GlobalOscCommandsConstants::projectChange) {
		if (!msg.arguments().isEmpty() && msg.arguments().first().type() == QVariant::String) {
			QString projectName = msg.arguments().first().toString();
			m_controller->projectManager()->setCurrentProject(projectName, false);
		}
	}
}
