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

#include "MidiManager.h"
#include "MainController.h"


MidiEvent MidiEvent::FromRawMessage(QString portName, std::vector<unsigned char>* message) {
	if (message->size() < 2) {
		// data is too short, return empty event:
		return MidiEvent {"", 0, 0, 0, 0, false};
	}

	// split message in type, channel, target and value:
	// (target is the Note key or the ControlChange target)
	int status = message->at(0);
	int channel = (status & 0x0f) + 1;
	int type = (status & 0xf0) >> 4;
	int target = message->at(1);

	// get value depending on type:
	double value;
	bool convertedFromNoteOff = false;
	if (type == MidiConstants::PROGRAM_CHANGE) {
		// Program Change doesn't have value, use program number as value:
		value = 1.0;
	} else if (type == MidiConstants::NOTE_OFF) {
		// convert note_off to note_on with value 0:
		type = MidiConstants::NOTE_ON;
		value = 0.0;
		convertedFromNoteOff = true;
	} else {
		// value is the last bytes last 7 bits:
		value = message->at(2) / 127.;
	}

	// create inputId from status and target:
	QString inputId = portName + QString::number(type) + QString::number(channel) + QString::number(target);

	MidiEvent event{inputId, value, type, channel, target, convertedFromNoteOff};
	return event;
}

// The following code will only be used when the RtMidi library is available on the platform.
// At the moment this is Windows, Linux and Mac.
// If not, a dummy class will be created below.

#ifdef RT_MIDI_AVAILABLE

// ----------------------- MidiInputDevice -----------------------

MidiInputDevice::MidiInputDevice(uint portNumber, QObject *parent)
	: QObject(parent)
{
	m_portNumber = portNumber;

	// create RtMidiIn object and set callback:
    try {
		m_input = new RtMidiIn();
		m_input->openPort(m_portNumber);
		m_portName = QString::fromStdString(m_input->getPortName(m_portNumber));
		m_input->setCallback( &staticMidiCallback, (void*)this );
		qDebug() << "MIDI input device found: " << m_portName;
    } catch ( RtMidiError &error ) {
		qWarning() << "An error occured while creating RtMidiIn object:";
        error.printMessage();
	}
}

MidiInputDevice::MidiInputDevice(QObject* parent)
	: QObject(parent)
{
	m_portNumber = -1;

	// create RtMidiIn object and set callback:
	try {
		m_input = new RtMidiIn();
		m_portName = "Luminosus In";
		m_input->openVirtualPort(m_portName.toStdString());
		m_input->setCallback( &staticMidiCallback, (void*)this );
	} catch ( RtMidiError &error ) {
		qWarning() << "An error occured while creating virtual RtMidiIn object:";
		error.printMessage();
	}
}

MidiInputDevice::~MidiInputDevice() {
	delete m_input;
	m_input = nullptr;
}

void MidiInputDevice::staticMidiCallback(double, std::vector<unsigned char> *message, void *instance) {
	// forward call to non-static method just for convenience:
	((MidiInputDevice*)instance)->rawMidiCallback(message);
}

void MidiInputDevice::rawMidiCallback(std::vector<unsigned char> *message) {
	MidiEvent event = MidiEvent::FromRawMessage(m_portName, message);
    emit eventReceived(event);
}



#endif // RT_MIDI_AVAILABLE


// ----------------------- MidiManager ------------------------------------

MidiManager::MidiManager(MainController *controller)
	: QObject(controller)
	, m_controller(controller)
	, m_nextEventCallbackIsValid(false)
	, m_default_input_channel(1)
	, m_default_output_channel(1)
	, m_log()
	, m_logInput(true)
	, m_logOutput(true)
{
	m_toneNames = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
	// Register ExternalInputEvent as Qt Meta Type to be able to store it in QVariants etc.:
	qRegisterMetaType<MidiEvent>("MidiEvent");
	// initialize:
	initializeInputs();
	initializeOutputs();
}

MidiManager::~MidiManager() {
#ifdef RT_MIDI_AVAILABLE
	// delete all created MidiInputDevice objects:
	for (auto input: m_inputs) {
		delete input;
		input = nullptr;
	}
	// delete all created RtMidiOut objects:
	for (auto output: m_outputs) {
		delete output;
		output = nullptr;
	}
#endif
}

void MidiManager::initializeInputs() {
#ifdef RT_MIDI_AVAILABLE
	RtMidiIn* midiin = 0;
	try {
		midiin = new RtMidiIn();
	} catch ( RtMidiError &error ) {
		qWarning() << "An error occured while creating RtMidiIn object:";
		error.printMessage();
		return;
	}
	if (!midiin) {
		qWarning() << "Could not create RtMidiIn object.";
		return;
	}
	unsigned int numberOfPorts = midiin->getPortCount();
	delete midiin;
	for ( unsigned int i=0; i<numberOfPorts; i++ ) {
		m_inputs.push_back(new MidiInputDevice(i, this));
		connect(m_inputs[i], SIGNAL(eventReceived(MidiEvent)), this, SLOT(onExternalEvent(MidiEvent)));
	}

#ifndef Q_OS_WIN  // virtual Midi Ports aren't possible with Windows and RtMidi
	// create virtual input port:
	MidiInputDevice* virtualInput = new MidiInputDevice(this);
	m_inputs.push_back(virtualInput);
	connect(virtualInput, SIGNAL(eventReceived(MidiEvent)), this, SLOT(onExternalEvent(MidiEvent)));
#endif  // Q_OS_WIN
#endif  // RT_MIDI_AVAILABLE
}

void MidiManager::initializeOutputs() {
#ifdef RT_MIDI_AVAILABLE
	RtMidiOut* midiout = 0;
	try {
		midiout = new RtMidiOut();
	} catch ( RtMidiError &error ) {
		qWarning() << "An error occured while creating RtMidiOut object:";
		error.printMessage();
		return;
	}
	if (!midiout) {
		qWarning() << "Could not create RtMidiOut object.";
		return;
	}
	unsigned int numberOfPorts = midiout->getPortCount();
	delete midiout;
	for (unsigned int i=0; i<numberOfPorts; ++i) {
		RtMidiOut* output = new RtMidiOut();
		if (!output) continue;
		output->openPort(i);
		// ignore default Windows Synth output:
		QString portName = QString::fromStdString(output->getPortName(i));
		if (portName.startsWith("Microsoft GS Wavetable Synth")) {
			delete output;
			continue;
		}
		m_outputs.push_back(output);
		qDebug() << "MIDI output device found: " << portName;
	}

#ifndef Q_OS_WIN  // virtual Midi Ports aren't possible with Windows and RtMidi
	// create virtual output port:
	RtMidiOut* output = new RtMidiOut();
	if (output) {
		output->openVirtualPort("Luminosus Out");
		m_outputs.push_back(output);
	}
#endif  // Q_OS_WIN
#endif  // RT_MIDI_AVAILABLE
}

void MidiManager::addToLog(bool out, QString text) const {
	if (out && m_logOutput) {
		QString time = "[" + QTime::currentTime().toString() + "] [Out] ";
		m_log.prepend(time + text);
		if (m_log.size() > MAX_LOG_LENGTH) m_log.removeLast();
		emit logChanged();
	} else if (!out && m_logInput) {
		QString time = "[" + QTime::currentTime().toString() + "] [In]  ";
		m_log.prepend(time + text);
		if (m_log.size() > MAX_LOG_LENGTH) m_log.removeLast();
		emit logChanged();
	}
}

void MidiManager::addToLog(bool out, int type, int channel, int target, double value) const {
	QString msg = "[CH " + QString::number(channel) + "]";
	switch (type) {
	case MidiConstants::NOTE_ON:
	{
		int tone = target % 12;
		QString octave = QString::number(target / 12);
		msg.append(" Note On  %1 %2 (%3) = %4");
		msg = msg.arg(m_toneNames[tone], octave, QString::number(target), QString::number(int(value * 127)));
		break;
	}
	case MidiConstants::NOTE_OFF:
	{
		int tone = target % 12;
		QString octave = QString::number(target / 12);
		msg.append(" Note Off %1 %2 (%3) = %4");
		msg = msg.arg(m_toneNames[tone], octave, QString::number(target), QString::number(int(value * 127)));
		break;
	}
	case MidiConstants::CONTROL_CHANGE:
		msg.append(" Control Change %1 = %2");
		msg = msg.arg(QString::number(target), QString::number(int(value * 127)));
		break;
	case MidiConstants::PROGRAM_CHANGE:
		msg.append(" Program Change %1");
		msg = msg.arg(QString::number(target + 1));
		break;
	default:
		msg.append(" Type: %1 Data1: %2 Data2: %3");
		msg = msg.arg(QString::number(type), QString::number(target), QString::number(int(value*127)));
		break;
	}
	addToLog(out, msg);
}

QJsonObject MidiManager::getState() {
	QJsonObject state;
	state["defaultInputChannel"] = getDefaultInputChannel();
	state["defaultOutputChannel"] = getDefaultOutputChannel();
	state["logInput"] = getLogInput();
	state["logOutput"] = getLogOutput();

	// save mapping:
	// TODO: comments
    QJsonObject mapping;
	for (auto entry: m_inputToControlMapping) {
        QJsonArray controls;
        for (QString controlUid: entry.second) {
            controls.append(controlUid);
        }
        if (controls.size() > 0) mapping[entry.first] = controls;
    }
	state["mapping"] = mapping;
	return state;
}

void MidiManager::setState(QJsonObject state) {
	setDefaultInputChannel(state["defaultInputChannel"].toInt());
	setDefaultOutputChannel(state["defaultOutputChannel"].toInt());
	setLogInput(state["logInput"].toBool());
	setLogOutput(state["logOutput"].toBool());

	// restore mapping:
	QJsonObject mapping = state["mapping"].toObject();
	// TODO: comments
	m_inputToControlMapping.clear();
    for (QJsonObject::iterator it = mapping.begin(); it != mapping.end(); it++) {
		m_inputToControlMapping[it.key()] = ControlCallbackList();
        for (QJsonValueRef ref: it.value().toArray()) {
			m_inputToControlMapping[it.key()].push_back(ref.toString());
        }
    }
}

void MidiManager::connectControl(QString controlUid, QString inputUid) {
	// check if there already is an entry for this input uid:
	if (!mapContains(m_inputToControlMapping, inputUid)) {
		// there is no entry -> create an empty list:
		m_inputToControlMapping[inputUid] = ControlCallbackList();
    }
	// append control uid to list if not already existing:
	appendUnique(m_inputToControlMapping[inputUid], controlUid);
}

void MidiManager::disconnectControl(QString controlUid) {
	// iterate over all entry in the mapping:
	for (auto& entry: m_inputToControlMapping) {
        ControlCallbackList& controlList = entry.second;
		// remove control uid if it exists:
        removeUnique(controlList, controlUid);
    }
}

void MidiManager::onExternalEvent(MidiEvent event) {
	// call nextEvent callback if there is one:
	if (m_nextEventCallbackIsValid) {
		m_nextEventCallback(event.inputId);
		m_nextEventCallbackIsValid = false;
    }

	// set "externalInput" property on controls that are mapped to this input:
	if (mapContains(m_inputToControlMapping, event.inputId)) {
		for (QString controlUid: m_inputToControlMapping[event.inputId]) {
			QQuickItem* control = m_controller->getControlFromUid(controlUid);
			// check if control still exists:
            if (!control) continue;
            control->setProperty("externalInput", event.value);
        }
    }

	// emit messageReceived signal i.e. for MidiBlock
	emit messageReceived(event);

	addToLog(false, event.convertedFromNoteOff ? MidiConstants::NOTE_OFF : event.type,
			 event.channel, event.target, event.value);
}

QStringList MidiManager::getToneNames() const {
	QStringList tones {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
	return tones;
}

void MidiManager::sendChannelVoiceMessage(unsigned char type, unsigned char channel, unsigned char target, double value) {
#ifdef RT_MIDI_AVAILABLE
	std::vector<unsigned char> message(3);
	message[0] = (type << 4) | (channel - 1);
	message[1] = target;
	message[2] = (unsigned char)(value * 127);
	for (RtMidiOut* output: m_outputs) {
		output->sendMessage(&message);
	}
	addToLog(true, type, channel, target, value);
#endif
}

void MidiManager::sendChannelVoiceMessage(unsigned char type, unsigned char channel, unsigned char target) {
#ifdef RT_MIDI_AVAILABLE
	std::vector<unsigned char> message(2);
	message[0] = (type << 4) | (channel - 1);
	message[1] = target;
	for (RtMidiOut* output: m_outputs) {
		output->sendMessage(&message);
	}
	if (m_logOutput) {
		addToLog(true, type, channel, target, 0);
	}
#endif
}




