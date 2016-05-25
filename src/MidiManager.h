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

#ifndef MIDIMANAGER_H
#define MIDIMANAGER_H

#include "utils.h"

#include <QObject>
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>

#include <vector>
#include <map>
#include <functional>

// forward declaration to reduce dependencies
class MainController;


namespace MidiConstants {
/**
 * @brief OMNI_MODE_CHANNEL is the "magic" channel number to use OMNI mode
 * @memberof MidiManager
 */
static const int OMNI_MODE_CHANNEL = 0;
/**
 * @brief NOTE_OFF is the Midi code for a Note Off message
 * @memberof MidiManager
 */
static const unsigned char NOTE_OFF = 0b1000;
/**
 * @brief NOTE_ON is the Midi code for a Note On message
 * @memberof MidiManager
 */
static const unsigned char NOTE_ON = 0b1001;
/**
 * @brief CONTROL_CHANGE is the Midi code for a Control Change message
 * @memberof MidiManager
 */
static const unsigned char CONTROL_CHANGE = 0b1011;
/**
 * @brief PROGRAM_CHANGE is the Midi code for a Program Change message
 * @memberof MidiManager
 */
static const unsigned char PROGRAM_CHANGE = 0b1100;
}


/**
 * @brief The MidiEvent struct represents an event coming
 * from an external device such as a MIDI controller.
 */
struct MidiEvent {
	/**
	 * @brief inputId is the unique identifier of this event source (i.e. the MIDI channel and note)
	 */
	QString inputId;

	/**
	 * @brief value of the input event between 0 and 1
	 */
    double value;

	/**
	 * @brief type Midi message type (Note On, Program Change etc.)
	 */
	int type;

	/**
	 * @brief channel Midi input channel starting with 1 [1-16]
	 */
	int channel;

	/**
	 * @brief target Midi target / first argument (Note, Program etc.) [0-127]
	 */
	int target;

	/**
	 * @brief convertedFromNoteOff true, if this is a Note On message with value 0.0
	 * converted from a Note Off message
	 */
	bool convertedFromNoteOff;

	/**
	 * @brief FromRawMessage creates a MidiEvent object from raw Midi data
	 * @param portName name of the port that received that event
	 * @param message raw data of the message
	 * @return a new Midi Event object
	 */
	static MidiEvent FromRawMessage(QString portName, std::vector<unsigned char>* message);
};



// The following code will only be used when the RtMidi library is available on the platform.
// At the moment this is Windows, Linux and Mac.
// If not, a dummy class will be created below.

#ifdef RT_MIDI_AVAILABLE

#include "RtMidi/RtMidi.h"



/**
 * @brief The MidiInputDevice class represents a single Midi input device.
 * It stores the name of the port and converts the raw messages to ExternalInputEvents.
 */
class MidiInputDevice : public QObject {

	Q_OBJECT

public:
	/**
	 * @brief MidiInputDevice creates an instance of this class and opens the given Midi port
	 * @param portNumber the number of the port in RtMidi
	 * @param parent QObject parent
	 */
	explicit MidiInputDevice(uint portNumber, QObject *parent = 0);
	/**
	 * @brief MidiInputDevice creates an instance of this class and opens a virtual input port
	 * @param parent QObject parent
	 */
	explicit MidiInputDevice(QObject *parent = 0);
	~MidiInputDevice();

	/**
	 * @brief staticMidiCallback is called by RtMidi with the raw data of an event
	 * @param message is the raw Midi data of this event
	 * @param instance is the instance of the MidiInputDevice class that is associated with the origin port
	 */
	static void staticMidiCallback(double, std::vector<unsigned char> *message, void *instance);

	/**
	 * @brief rawMidiCallback converts a raw Midi message to an ExternalInputEvent struct
	 * and emits the eventReceived signal
	 * @param message is the raw Midi data
	 */
    void rawMidiCallback(std::vector<unsigned char>* message);

signals:
	/**
	 * @brief eventReceived will be emitted when a Midi event has been received
	 * @param event is the event that has been received (with a normalized value)
	 */
	void eventReceived(MidiEvent event);

protected:

	/**
	 * @brief m_input is the associated RtMidiIn object
	 */
	RtMidiIn* m_input;

	/**
	 * @brief m_portNumber is the number of the Midi port in RtMidi
	 */
	uint m_portNumber;

	/**
	 * @brief m_portName is the human readable name of the Midi port
	 */
	QString m_portName;
};

#endif // RT_MIDI_AVAILABLE


// The following code declares a dummy MidiInputDevice class
// that is used on devices where Midi is not available (i.e. Android and iOS).

#ifndef RT_MIDI_AVAILABLE  // i.e. on Android / iOS

class MidiInputDevice : public QObject {
	Q_OBJECT

public:
	explicit MidiInputDevice(uint /*portNumber*/, QObject *parent = 0)
		: QObject(parent)
	{}
	explicit MidiInputDevice(QObject *parent = 0)
		: QObject(parent)
	{}

	~MidiInputDevice() {}

	static void staticMidiCallback(double, std::vector<unsigned char> * /*message*/, void */*userData*/) {}

	void rawMidiCallback(std::vector<unsigned char>* /*message*/) {}

signals:
    void eventReceived(MidiEvent event);
};

#endif // not RT_MIDI_AVAILABLE

// -------------------------------------------------------------------------

/**
 * @brief The MidiManager class manages all MidiInputDevice objects.
 */
class MidiManager : public QObject {

	Q_OBJECT

	Q_PROPERTY(int defaultInputChannel READ getDefaultInputChannel WRITE setDefaultInputChannel NOTIFY defaultInputChannelChanged)
	Q_PROPERTY(int defaultOutputChannel READ getDefaultOutputChannel WRITE setDefaultOutputChannel NOTIFY defaultOutputChannelChanged)

	Q_PROPERTY(bool logInput READ getLogInput WRITE setLogInput NOTIFY logInputChanged)
	Q_PROPERTY(bool logOutput READ getLogOutput WRITE setLogOutput NOTIFY logOutputChanged)

public:
	/**
	 * @brief MidiManager creates a MidiManager that creates MidiInputDevice objects for each Midi device
	 * @param controller pointer to the MainController
	 */
	explicit MidiManager(MainController* m_controller);
	~MidiManager();

	/**
	 * @brief getState returns the current state to persist it
	 * @return state as Json object
	 */
	QJsonObject getState();
	/**
	 * @brief setState restores a saved state
	 * @param state Json object
	 */
	void setState(QJsonObject state);

signals:
	/**
	 * @brief messageReceived emitted when a Midi message is received
	 * @param event the Midi message as a MidiEvent object
	 */
	void messageReceived(MidiEvent event);

	/**
	 * @brief defaultInputChannelChanged emitted when the default input channel changed
	 */
	void defaultInputChannelChanged();
	/**
	 * @brief defaultOutputChannelChanged emitted when the default output channel changed
	 */
	void defaultOutputChannelChanged();

	/**
	 * @brief logChanged emitted when a message was added to the log
	 */
	void logChanged() const;
	/**
	 * @brief logInputChanged emitted when the logInput settings changed
	 */
	void logInputChanged();
	/**
	 * @brief logOutputChanged emitted when the logOutput settings changed
	 */
	void logOutputChanged();

public slots:

	/**
	 * @brief registerForNextEvent register a callback that is called when the next event is received
	 * @param callback function taking a string with the uid of the next callback
	 */
    void registerForNextEvent(std::function<void(QString)> callback) {
		m_nextEventCallback = callback;
		m_nextEventCallbackIsValid = true;
    }

	/**
	 * @brief clearNextEventCallback removes the callback for the next event if there was one
	 */
	void clearNextEventCallback() {
		m_nextEventCallback = [](QString) {};
		m_nextEventCallbackIsValid = false;
    }

	/**
	 * @brief connectControl connects a control with an input event
	 * @param controlUid the uid of the control
	 * @param inputUid the uid of the input event
	 */
    void connectControl(QString controlUid, QString inputUid);

	/**
	 * @brief disconnectControl disconnects a control from all connected input events
	 * @param controlUid the uid of the control
	 */
    void disconnectControl(QString controlUid);

	/**
	 * @brief onExternalEvent handles an incoming input event and calls associated callbacks
	 * @param event the incoming input event
	 */
	void onExternalEvent(MidiEvent event);

	/**
	 * @brief getDefaultInputChannel returns the default Midi input channel
	 * @return input channel number [1-16] or 0 for OMNI mode
	 */
	int getDefaultInputChannel() const { return m_default_input_channel; }
	/**
	 * @brief setDefaultInputChannel sets the default Midi input channel
	 * @param value channel number [1-16] or 0 for OMNI mode
	 */
	void setDefaultInputChannel(int value) { m_default_input_channel = limit(0, value, 16); emit defaultInputChannelChanged(); }

	/**
	 * @brief getDefaultOutputChannel returns the default Midi output channel
	 * @return output channel number [1-16]
	 */
	int getDefaultOutputChannel() const { return m_default_output_channel; }
	/**
	 * @brief setDefaultOutputChannel sets the default Midi output channel
	 * @param value channel number [1-16]
	 */
	void setDefaultOutputChannel(int value) { m_default_output_channel = limit(1, value, 16); emit defaultOutputChannelChanged(); }

	/**
	 * @brief sendChannelVoiceMessage sends a MIDI channel voice message with two arguments (Note or Control)
	 * @param type Midi code for message type (see MidiConstants)
	 * @param channel midi output channel
	 * @param target first argument of the message (i.e. note)
	 * @param value second argument of the message (i.e. velocity)
	 */
	void sendChannelVoiceMessage(unsigned char type, unsigned char channel, unsigned char target, double value);

	/**
	 * @brief sendChannelVoiceMessage sends a MIDI channel voice message with one argument (Program Change)
	 * @param type Midi code for message type (see MidiConstants)
	 * @param channel midi output channel
	 * @param target first argument of the message (i.e. program)
	 */
	void sendChannelVoiceMessage(unsigned char type, unsigned char channel, unsigned char target);

	/**
	 * @brief getToneNames returns a list of the names of tones in an octave
	 * @return a list of tone names
	 */
	QStringList getToneNames() const;

	// ------------------- Logging --------------------

	/**
	 * @brief getLog returns the log as a QStringList to be displayed in UI
	 * @return log as QStringList
	 */
	QStringList getLog() const { return m_log; }

	/**
	 * @brief getLogInput returns if logging of incoming messages is enabled
	 * @return true if enabled
	 */
	bool getLogInput() const { return m_logInput; }
	/**
	 * @brief setLogInput enables or disables logging of incoming messages
	 * @param value true to enable it
	 */
	void setLogInput(bool value) { m_logInput = value; emit logInputChanged(); }

	/**
	 * @brief getLogOutput returns if logging of outgoing messages is enabled
	 * @return true if enabled
	 */
	bool getLogOutput() const { return m_logOutput; }
	/**
	 * @brief setLogOutput enables or disables logging of outgoing messages
	 * @param value true to enable it
	 */
	void setLogOutput(bool value) { m_logOutput = value; emit logOutputChanged(); }

	/**
	 * @brief clearLog clears the log
	 */
	void clearLog() const { m_log.clear(); emit logChanged(); }

private:
	/**
	 * @brief initializeInputs is called by ctor and creates MidiInputDevice objects for each Midi device
	 */
	void initializeInputs();

	/**
	 * @brief initializeOutputs is called by ctor and creates RtMidiOut objects for each Midi device
	 */
	void initializeOutputs();


protected:

	/**
	 * @brief addToLog adds a text to the log
	 * @param text to add to the log
	 */
	void addToLog(bool out, QString text) const;

	/**
	 * @brief addToLog adds a Midi message to the log
	 * @param out true if it is an outgoing message
	 * @param type Midi code of the message
	 * @param channel Midi channel
	 * @param target first arguement
	 * @param value second argument
	 */
	void addToLog(bool out, int type, int channel, int target, double value) const;

	/**
	 * @brief m_controller a pointer to the MainController
	 */
	MainController*	m_controller;

	/**
	 * @brief m_inputs list of MidiInputDevice objects
	 */
	std::vector<MidiInputDevice*> m_inputs;

#ifdef RT_MIDI_AVAILABLE
	/**
	 * @brief m_outputs is the list of the created RtMidiOut objects
	 */
	std::vector<RtMidiOut*> m_outputs;
#endif
#ifndef RT_MIDI_AVAILABLE
	/**
	 * @brief m_outputs is a dummy of the RtMidiOut object list on platforms without Midi support
	 */
    std::vector<void*> m_outputs;
#endif

	/**
	 * @brief m_nextEventCallback the callback to be called when the next event is received
	 */
	std::function<void(QString)> m_nextEventCallback;

	/**
	 * @brief m_nextEventCallbackIsValid true, if the m_nextEventCallback is valid and should be called next time
	 */
	bool m_nextEventCallbackIsValid;

	/**
	 * @brief ControlCallbackList is the type used for the list of callbacks for a specific input
	 */
	typedef std::vector<QString> ControlCallbackList;
	/**
	 * @brief m_inputToControlMapping the mapping of input events to controls
	 */
	std::map<QString, ControlCallbackList> m_inputToControlMapping;

	/**
	 * @brief m_default_input_channel is the default Midi input channel [1-16] or 0 for OMNI mode
	 */
	int m_default_input_channel;

	/**
	 * @brief m_default_output_channel is the default Midi output channel [1-16]
	 */
	int m_default_output_channel;

	/**
	 * @brief log of incoming and / or outgoing messages
	 */
	mutable QStringList m_log;
	/**
	 * @brief m_logInput is true if incoming messages should be logged
	 */
	bool m_logInput;
	/**
	 * @brief m_logOutput is true if outgoing messages should be logged
	 */
	bool m_logOutput;

	/**
	 * @brief m_toneNames is the list of the names of the tones in an octave
	 */
	std::vector<QString> m_toneNames;

};

#endif // MIDIMANAGER_H
