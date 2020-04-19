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
#include "core/MainController.h"


MidiEvent MidiEvent::FromRawMessage(QString /*portName*/, std::vector<unsigned char>* message) {
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

    QString inputId = "%1%2%3";
    inputId = inputId.arg(type, 3, 10, QChar('0'));
    inputId = inputId.arg(channel, 3, 10, QChar('0'));
    inputId = inputId.arg(target, 3, 10, QChar('0'));
    //inputId = inputId.arg(portName);
    // -> don't include port name because it changes by the count of connected devices

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
        m_input->setCallback(&staticMidiCallback, static_cast<void*>(this));
    } catch ( RtMidiError &error ) {
		qCritical() << "An error occured while creating RtMidiIn object:";
        error.printMessage();
	}
}

MidiInputDevice::MidiInputDevice(QObject* parent)
	: QObject(parent)
{
    m_portNumber = 0;

	// create RtMidiIn object and set callback:
	try {
		m_input = new RtMidiIn();
		m_portName = "Luminosus In";
		m_input->openVirtualPort(m_portName.toStdString());
        m_input->setCallback(&staticMidiCallback, static_cast<void*>(this));
	} catch ( RtMidiError &error ) {
		qCritical() << "An error occured while creating virtual RtMidiIn object:";
		error.printMessage();
	}
}

MidiInputDevice::~MidiInputDevice() {
	delete m_input;
	m_input = nullptr;
}

void MidiInputDevice::staticMidiCallback(double, std::vector<unsigned char>* message, void* instance) {
	// forward call to non-static method just for convenience:
    MidiInputDevice* device = static_cast<MidiInputDevice*>(instance);
    if (device) {
        device->rawMidiCallback(message);
    }
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
    , m_defaultInputChannel(1)
    , m_defaultOutputChannel(1)
	, m_log()
	, m_logInput(true)
    , m_logOutput(true)
    , m_autoRefresh(false)
    , m_hiddenInputPorts(0)
    , m_hiddenOutputPorts(0)
{
    // prepare log changed signal:
    m_logChangedSignalDelay.setSingleShot(true);
    m_logChangedSignalDelay.setInterval(100);
    connect(&m_logChangedSignalDelay, SIGNAL(timeout()), this, SIGNAL(logChanged()));
    connect(this, SIGNAL(autoRefreshChanged()), this, SLOT(switchAutoRefresh()));
    connect(&m_autoRefreshTimer, SIGNAL(timeout()), this, SLOT(refreshDevices()));

    // prepare Port Blacklist:
    m_portNameBlacklist << "Microsoft GS Wavetable Synth"
                        << "Luminosus In"
                        << "Luminosus Out"
                        << "RtMidi"
                        << "X18/XR18"
                        << "Midi Through";
	m_toneNames = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
	// Register ExternalInputEvent as Qt Meta Type to be able to store it in QVariants etc.:
	qRegisterMetaType<MidiEvent>("MidiEvent");

    // initialize:
    initializeInputs();
    initializeOutputs();

    emit autoRefreshChanged();
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
#ifndef Q_OS_WIN  // virtual Midi Ports aren't possible with Windows and RtMidi
	// create virtual input port:
	MidiInputDevice* virtualInput = new MidiInputDevice(this);
	m_inputs.push_back(virtualInput);
	connect(virtualInput, SIGNAL(eventReceived(MidiEvent)), this, SLOT(onExternalEvent(MidiEvent)));
    m_hiddenInputPorts++;
#endif  // Q_OS_WIN

    refreshInputs();

#endif  // RT_MIDI_AVAILABLE
}

void MidiManager::initializeOutputs() {
#ifdef RT_MIDI_AVAILABLE
#ifndef Q_OS_WIN  // virtual Midi Ports aren't possible with Windows and RtMidi
	// create virtual output port:
	RtMidiOut* output = new RtMidiOut();
	if (output) {
		output->openVirtualPort("Luminosus Out");
		m_outputs.push_back(output);
        m_hiddenOutputPorts++;
	}
#endif  // Q_OS_WIN

    refreshOutputs();

#endif  // RT_MIDI_AVAILABLE
}

void MidiManager::refreshInputs() {
#ifdef RT_MIDI_AVAILABLE
    RtMidiIn* midiin = nullptr;
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
    QVector<QString> availablePorts;
    QVector<unsigned int> portIdx;
    unsigned int numberOfPorts = midiin->getPortCount();
    for ( unsigned int i=0; i<numberOfPorts; i++ ) {
        QString portName = QString::fromStdString(midiin->getPortName(i));

        // ignore blacklisted ports:
        bool blacklisted = false;
        for (QString blacklistedName: m_portNameBlacklist) {
            if (portName.startsWith(blacklistedName)) {
                blacklisted = true;
                break;
            }
        }
        if (blacklisted) continue;

        availablePorts.push_back(portName);
        portIdx.push_back(i);
    }
    QVector<unsigned int> toDelete;
    for (int i = 0; i < m_inputPortNames.size(); i++) {
        if (!availablePorts.contains(m_inputPortNames[i])) {
            toDelete.push_back((unsigned int) i);
        }
    }
    for (auto idx = toDelete.size() - 1; idx >=0; idx--) {
        unsigned int i = toDelete[idx];
            if (m_inputs.size() > i + m_hiddenInputPorts) {
                disconnect(m_inputs[i + m_hiddenInputPorts], SIGNAL(eventReceived(MidiEvent)), this, SLOT(onExternalEvent(MidiEvent)));
                delete m_inputs[i + m_hiddenInputPorts];
                m_inputs.erase(m_inputs.begin() + i  + m_hiddenInputPorts);
                m_inputPortNames.remove((int) i);
            }
    }
    for (int i = 0; i < availablePorts.size(); i++) {
        QString portName = availablePorts[i];

        // check if port is already opened:
        if (m_inputPortNames.contains(portName)) {
            continue;
        }
        MidiInputDevice* input = new MidiInputDevice(portIdx[i], this);
        m_inputs.push_back(input);
        m_inputPortNames.push_back(portName);
        connect(input, SIGNAL(eventReceived(MidiEvent)), this, SLOT(onExternalEvent(MidiEvent)));
    }
    emit portNamesChanged();
    delete midiin;
#endif  // RT_MIDI_AVAILABLE
}

void MidiManager::refreshOutputs() {
#ifdef RT_MIDI_AVAILABLE
    RtMidiOut* midiout = nullptr;
    try {
        midiout = new RtMidiOut();
    } catch ( RtMidiError &error ) {
        qCritical() << "An error occured while creating RtMidiOut object:";
        error.printMessage();
        return;
    }
    if (!midiout) {
        qCritical() << "Could not create RtMidiOut object.";
        return;
    }
    QVector<QString> availablePorts;
    QVector<unsigned int> portIdx;
    unsigned int numberOfPorts = midiout->getPortCount();
    for (unsigned int i=0; i<numberOfPorts; ++i) {
        QString portName = QString::fromStdString(midiout->getPortName(i));

        // ignore blacklisted ports:
        bool blacklisted = false;
        for (QString blacklistedName: m_portNameBlacklist) {
            if (portName.startsWith(blacklistedName)) {
                blacklisted = true;
                break;
            }
        }
        if (blacklisted) continue;

        availablePorts.push_back(portName);
        portIdx.push_back(i);
    }
    QVector<unsigned int> toDelete;
    for (int i = 0; i < m_outputPortNames.size(); i++) {
        if (!availablePorts.contains(m_outputPortNames[i])) {
            toDelete.push_back((unsigned int) i);
        }
    }
    for (auto idx = toDelete.size() - 1; idx >=0; idx--) {
        unsigned int i = toDelete[idx];
            if (m_outputs.size() > i + m_hiddenOutputPorts) {
                m_outputs[i + m_hiddenOutputPorts]->closePort();
                delete m_outputs[i + m_hiddenOutputPorts];
                m_outputs.erase(m_outputs.begin() + i  + m_hiddenOutputPorts);
                m_outputPortNames.remove((int) i);
            }
    }
    for (int i = 0; i < availablePorts.size(); i++) {
        QString portName = availablePorts[i];

        // check if port is already opened:
        if (m_outputPortNames.contains(portName)) {
            continue;
        }

        RtMidiOut* output = new RtMidiOut();
        if (!output) continue;

        try {
            output->openPort(portIdx[i]);
        } catch ( RtMidiError &error ) {
            qCritical() << "An error occured while opening RtMidiOut port:";
            error.printMessage();
            return;
        }

        m_outputs.push_back(output);
        m_outputPortNames.push_back(portName);
    }
    emit portNamesChanged();
    delete midiout;
#endif  // RT_MIDI_AVAILABLE
}

void MidiManager::switchAutoRefresh() {
    if (m_autoRefresh) {
        m_autoRefreshTimer.setSingleShot(false);
        m_autoRefreshTimer.start(500);
    } else
        m_autoRefreshTimer.stop();
}

void MidiManager::addToLog(bool out, QString text) const {
	if (out && m_logOutput) {
		QString time = "[" + QTime::currentTime().toString() + "] [Out] ";
		m_log.prepend(time + text);
		if (m_log.size() > MAX_LOG_LENGTH) m_log.removeLast();
        m_logChangedSignalDelay.start();
	} else if (!out && m_logInput) {
		QString time = "[" + QTime::currentTime().toString() + "] [In]  ";
		m_log.prepend(time + text);
		if (m_log.size() > MAX_LOG_LENGTH) m_log.removeLast();
        m_logChangedSignalDelay.start();
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

QJsonObject MidiManager::getState() const {
	QJsonObject state;
	state["defaultInputChannel"] = getDefaultInputChannel();
	state["defaultOutputChannel"] = getDefaultOutputChannel();
	state["logInput"] = getLogInput();
	state["logOutput"] = getLogOutput();
    state["autoRefresh"] = getAutoRefresh();
	return state;
}

void MidiManager::setState(QJsonObject state) {
	setDefaultInputChannel(state["defaultInputChannel"].toInt());
	setDefaultOutputChannel(state["defaultOutputChannel"].toInt());
	setLogInput(state["logInput"].toBool());
	setLogOutput(state["logOutput"].toBool());
    setAutoRefresh(state["autoRefresh"].toBool());
}

void MidiManager::registerForNextEvent(QString id, const MidiConstants::NextEventCallback& callback) {
    m_nextEventCallbacks[id] = callback;
}

void MidiManager::clearNextEventCallbacks() {
    m_nextEventCallbacks.clear();
}

void MidiManager::removeNextEventCallback(QString id) {
    m_nextEventCallbacks.remove(id);
}

QStringList MidiManager::getToneNames() const {
	QStringList tones {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
	return tones;
}

void MidiManager::refreshDevices() {
    refreshInputs();
    refreshOutputs();
}

void MidiManager::sendChannelVoiceMessage(unsigned char type, unsigned char channel, unsigned char target, double value) {
#ifdef RT_MIDI_AVAILABLE
    static std::vector<unsigned char> message(3);
	message[0] = (type << 4) | (channel - 1);
	message[1] = target;
    message[2] = static_cast<unsigned char>(value * 127);
	for (RtMidiOut* output: m_outputs) {
        try {
            output->sendMessage(&message);
//            std::cout << "M" << message[2] << std::endl;
        } catch ( RtMidiError& ) {
            qWarning("MIDI device not available (probably disconnect).");
            output->closePort();
        }
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
        try {
            output->sendMessage(&message);
        } catch ( RtMidiError& ) {
            qWarning("MIDI device not available (probably disconnect).");
            output->closePort();
        }
	}
	if (m_logOutput) {
		addToLog(true, type, channel, target, 0);
	}
#endif
}

QString MidiManager::getFeedbackAddress(unsigned char type, unsigned char channel, unsigned char target) const {
    QByteArray address;
    address.append(type);
    address.append(channel);
    address.append(target);
    // convert to Base64 because it will be saved in a JSON file later:
    return QString(address.toBase64());
}

void MidiManager::sendFeedback(QString addressString, double value) {
    QByteArray address = QByteArray::fromBase64(addressString.toLatin1());

    unsigned char type = address[0];
    unsigned char channel = address[1];
    unsigned char target = address[2];

    sendChannelVoiceMessage(type, channel, target, value);
}

void MidiManager::onExternalEvent(MidiEvent event) {
    // call nextEvent callback if there is one:
    if (!m_nextEventCallbacks.isEmpty()) {
        for (auto callback: m_nextEventCallbacks) {
            callback(event);
        }
        m_nextEventCallbacks.clear();
    }

    // emit messageReceived signal i.e. for MidiBlock
    emit messageReceived(event);

    addToLog(false, event.convertedFromNoteOff ? MidiConstants::NOTE_OFF : event.type,
             event.channel, event.target, event.value);
}




