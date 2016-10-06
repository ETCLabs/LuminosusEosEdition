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

#ifndef OSCWRAPPER_H
#define OSCWRAPPER_H

#include "OSCParser.h"
#include "OSCMessage.h"
#include "utils.h"

#include <QObject>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QTimer>
#include <QJsonObject>


/**
 * @brief time to try to connect again after an error in ms
 * @memberof OSCNetworkManager
 */
static const int TRY_CONNECT_AGAIN_TIME = 3000;  // ms

/**
 * @brief default TCP port
 * @memberof OSCNetworkManager
 */
static const quint16 DEFAULT_TCP_PORT = 3032;

/**
 * @brief default UDP Tx port
 * @memberof OSCNetworkManager
 */
static const quint16 DEFAULT_UDP_TX_PORT = 8001;

/**
 * @brief default UDP Rx port
 * @memberof OSCNetworkManager
 */
static const quint16 DEFAULT_UDP_RX_PORT = 8000;

/**
 * @brief maximum entries in OSC log
 * @memberof OSCNetworkManager
 */
static const int MAX_LOG_LENGTH = 1000;

/**
 * @brief PROTOCOL_UDP is the human readable name of this protocol
 * @memberof OSCNetworkManager
 */
static const QString PROTOCOL_UDP = "UDP";
/**
 * @brief PROTOCOL_TCP_1_0 is the human readable name of this protocol
 * @memberof OSCNetworkManager
 */
static const QString PROTOCOL_TCP_1_0 = "TCP 1.0";
/**
 * @brief PROTOCOL_TCP_1_1 is the human readable name of this protocol
 * @memberof OSCNetworkManager
 */
static const QString PROTOCOL_TCP_1_1 = "TCP 1.1";


/**
 * @brief The OSCNetworkManager class manages OSC data exchange.
 * It can send and receive OSC messages via UDP and TCP
 * and supports OSC 1.0 and 1.1 packet-framing.
 */
class OSCNetworkManager : public QObject
{
	Q_OBJECT

public:
    /**
     * @brief OSCNetworkManager creates an OSCNetworkManager object
     */
	OSCNetworkManager();

public slots:

	// ------------------- Getter / Setter --------------------

	/**
	 * @brief getIpAddress returns the IP address used for UDP and TCP
	 * @return an IP address
	 */
	QHostAddress getIpAddress() const { return m_ipAddress; }
	/**
	 * @brief getIpAddressString returns the IP address as a String
	 * @return an IP address string
	 */
	QString getIpAddressString() const { return m_ipAddress.toString(); }
	/**
	 * @brief setIpAddress sets the IP address used for UDP and TCP
	 * @param value the IP address
	 */
	void setIpAddress(const QHostAddress& value);
	/**
	 * @brief setIpAddress sets the IP address used for UDP and TCP
	 * @param value the IP address
	 */
	void setIpAddress(const QString& value);

	/**
	 * @brief getUdpTxPort returns the port to send UDP messages to
	 * @return a UDP port number
	 */
	quint16 getUdpTxPort() const { return m_udpTxPort; }
	/**
	 * @brief setUdpTxPort sets the port to send UDP messages to
	 * @param value the port number
	 */
	void setUdpTxPort(const quint16& value) { m_udpTxPort = limit(0, value, 65535); updateUdpBinding(); emit addressChanged(); }

	/**
	 * @brief getUdpRxPort returns the port to receive UDP messages from
	 * @return a UDP port
	 */
	quint16 getUdpRxPort() const { return m_udpRxPort; }
	/**
	 * @brief setUdpRxPort sets the port to receive UDP messages from
	 * @param value the port number
	 */
	void setUdpRxPort(const quint16& value) { m_udpRxPort = limit(0, value, 65535); updateUdpBinding(); emit addressChanged(); }

	/**
	 * @brief getTcpPort returns the port to send and receive TCP messages
	 * @return a port number
	 */
	quint16 getTcpPort() const { return m_tcpPort; }
	/**
	 * @brief setTcpPort sets the port to send and receive TCP messages
	 * @param value the port number
	 */
	void setTcpPort(const quint16& value);

	/**
	 * @brief getEnabled returns if OSC output is enabled
	 * @return true if OSC trigger output is enabled
	 */
	bool getEnabled() const { return m_isEnabled; }
	/**
	 * @brief setEnabled sets if OSC output is enabled
	 * @param value true to enable
	 */
	void setEnabled(bool value) { m_isEnabled = value; }

	/**
	 * @brief getEosUser returns the user used for Eos messages
	 * @return the number of the user as a string
	 */
	QString getEosUser() const { return m_eosUser; }
	/**
	 * @brief setEosUser sets the user used for Eos messages
	 * @param value user number as string (default is 0 -> Background User)
	 */
	void setEosUser(QString value) { m_eosUser = value; }

	/**
	 * @brief getUseTcp returns if TCP is used to send OSC messages
	 * @return true if TCP is used
	 */
	bool getUseTcp() const { return m_useTcp; }
	/**
	 * @brief setUseTcp sets if TCP is used to send OSC messages
	 * @param value true to use TCP
	 */
	void setUseTcp(bool value);

	/**
	 * @brief getUseOsc_1_1 returns if OSC 1.1 is used (SLIP-Framing)
	 * @return true if 1.1 is used
	 */
	bool getUseOsc_1_1() const;
	/**
	 * @brief setUseOsc_1_1 sets if OSC 1.1 is used (SLIP-Framing)
	 * @param value true to use 1.1 (default is false)
	 */
	void setUseOsc_1_1(bool value);

	/**
	 * @brief isConnected returns if the TCP socket is connected, returns true if UDP is used
	 * @return true if in "connected" state
	 */
	bool isConnected() const;

    /**
     * @brief setEosConnectionDefaults sets all parameters to the default values for a connection
     * to an Eos console
     */
    void setEosConnectionDefaults();

	// ------------------- Logging --------------------

	/**
	 * @brief getLog returns the log as a QStringList to be displayed in UI
	 * @return log as QStringList
	 */
	QStringList getLog() const { return m_log; }

	/**
	 * @brief enableLogging enables logging separatly for incoming and outgoing messages
	 * @param incoming true to enable logging of incoming messages
	 * @param outgoing true to enable logging of outgoing messages
	 */
	void enableLogging(bool incoming, bool outgoing);

	/**
	 * @brief getLogIncomingIsEnabled returns if logging of incoming messages is enabled
	 * @return true if enabled
	 */
	bool getLogIncomingIsEnabled() const { return m_logIncomingMsg; }

	/**
	 * @brief getLogOutgoingIsEnabled returns if logging of outgoing messages is enabled
	 * @return true if enabled
	 */
	bool getLogOutgoingIsEnabled() const { return m_logOutgoingMsg; }

	/**
	 * @brief clearLog clears the log
	 */
	void clearLog() const { m_log.clear(); emit logChanged(); }

	// ------------------- Send Message --------------------

	/**
	 * @brief Sends an OSC packet
	 * @param messageString path and arguments as a string in the format: /x/y/z=1.0,2.0
	 * @param forced true to send message even if OSC output is disabled
	 */
	void sendMessage(QString messageString, bool forced = false);

	/**
	 * @brief Sends an OSC message with a string as the only argument
	 * @param path of the message
	 * @param argument a string to be sent as the only argument
	 * @param forced true to send message even if OSC output is disabled
	 */
	void sendMessage(QString path, QString argument, bool forced = false);

    /**
     * @brief Sends an OSC message with a float as the only argument
     * @param path of the message
     * @param argument a float to be sent as the only argument
     * @param forced true to send message even if OSC output is disabled
     */
    void sendMessage(QString path, qreal argument, bool forced = false);

    /**
     * @brief Sends an OSC message with two floats as the arguments
     * @param path of the message
     * @param argument1 a float to be sent as the first argument
     * @param argument2 a float to be sent as the second argument
     * @param forced true to send message even if OSC output is disabled
     */
    void sendMessage(QString path, qreal argument1, qreal argument2, bool forced = false);

    /**
     * @brief Sends an OSC message with two strings as the arguments
     * @param path of the message
     * @param argument1 a string to be sent as the first argument
     * @param argument2 a string to be sent as the second argument
     * @param forced true to send message even if OSC output is disabled
     */
    void sendMessage(QString path, QString argument1, QString argument2, bool forced = false);


	// ------------------- GUI convenience functions --------------------

	/**
	 * @brief getProtocolNames returns a list of all available protocols
	 * @return a list of strings
	 */
	QStringList getProtocolNames() const;

	/**
	 * @brief getProtocolString returns the currently used protocol as a string
	 * @return an entry of getProtocolNames()
	 */
	QString getProtocolString() const;

	/**
	 * @brief setProtocolByString sets the used protocol by a string
	 * @param value an entry of getProtocolNames()
	 */
	void setProtocolByString(const QString& value);


	// ------------------- Persistence --------------------

	/**
	 * @brief getState returns the settings of this manager to persist them
	 * @return the settings as a QJsonObject
	 */
	QJsonObject getState() const;

	/**
	 * @brief setState restores the settings from a saved Json object
	 * @param state a QJsonObject previously generated by getState()
	 */
	void setState(const QJsonObject& state);

signals:

	// ------------------- Receive Message --------------------

	/**
	 * @brief messageReceived emitted when an OSC messages has been received
	 * @param msg the received message
	 */
	void messageReceived(OSCMessage msg);

	// ------------------- Status --------------------

	/**
	 * @brief will be emitted when a packet is successfully sent
	 */
	void packetSent();

	/**
	 * @brief will be emitted when the protocol (UDP/TCP or OSC version) changed
	 */
	void protocolChanged();

	/**
	 * @brief will be emitted when the connection state changed
	 */
	void isConnectedChanged();

	/**
	 * @brief will be emitted when the IP address or a port has changed
	 */
	void addressChanged();

	/**
	 * @brief will be emitted when the log has changed
	 */
	void logChanged() const;


protected:

	// ------------------- Private / Internal --------------------

	/**
	 * @brief reconnects to host if TCP is used
	 */
	void reconnect();

	/**
	 * @brief updateUdpBinding binds the UDP socket to the correct port
	 * or disables the binding if TCP is used
	 */
	void updateUdpBinding();

	/**
	 * @brief sendMessageData sends raw OSC message data
	 * @param packet OSC message as an array of chars
	 * @param outSize size of the packet
	 */
	void sendMessageData(char* packet, size_t outSize);

	/**
	 * @brief popPacketFromStreamData returns and removes the
	 * raw OSC message data from a framed packet in a TCP stream
	 * or returns nothing if the OSC message is not yet complete.
	 * Discards the data if it is invalid.
	 * @param data the incoming stream data (will be modified)
	 * @return the data of a single OSC message
	 */
	QByteArray popPacketFromStreamData(QByteArray& data) const;
	/**
	 * @brief returns and removes the raw OSC message data from
	 * a "packet-length-framed" packet in a TCP stream
	 * or returns nothing if the OSC message is not yet complete.
	 * Discards the data if it is invalid.
	 * @param data the incoming stream data (will be modified)
	 * @return the data of a single OSC packet (could be a bundle or message)
	 */
	QByteArray popPacketLengthFramedPacketFromStreamData(QByteArray& data) const;
	/**
	 * @brief returns and removes the raw OSC message data from
	 * a SLIP framed packet in a TCP stream
	 * or returns nothing if the OSC message is not yet complete.
	 * Discards the data if it is invalid.
	 * @param data the incoming stream data (will be modified)
	 * @return the data of a single OSC packet (could be a bundle or message)
	 */
	QByteArray popSlipFramedPacketFromStreamData(QByteArray& data) const;

	/**
	 * @brief addToLog adds a text to the log
	 * @param out true, if it was an outgoing message
	 * @param text to add to the log
	 */
	void addToLog(bool out, QString text) const;

private slots:

	/**
	 * @brief tryToConnectTCP tries to establish a connection via TCP
	 */
	void tryToConnectTCP();

	/**
	 * @brief onConnected is called when the TCP socket is connected, only for logging
	 */
	void onConnected();

	/**
	 * @brief onError is called when an error occurs while connected via TCP.
	 * starts a timer to retry the connection
	 */
	void onError();

	/**
	 * @brief readIncomingUdpDatagrams processes incoming UDP datagrams
	 */
	void readIncomingUdpDatagrams();

	/**
	 * @brief readIncomingTcpStream processes incoming TCP stream
	 */
	void readIncomingTcpStream();

	/**
	 * @brief processIncomingRawData processes incoming raw data and checks if it is an OSC bundle
	 * @param msgData raw OSC packet data (bundle or message)
	 */
	void processIncomingRawData(QByteArray msgData);

	/**
	 * @brief processIncomingRawMessage processes incoming single raw OSC messages
	 * @param msgData raw OSC message data (not a bundle)
	 */
	void processIncomingRawMessage(QByteArray msgData);

private:
	/**
	 * @brief UDP socket
	 */
	QUdpSocket				m_udpSocket;
	/**
	 * @brief TCP socket
	 */
	QTcpSocket				m_tcpSocket;
	/**
	 * @brief IP address for UDP and TCP
	 */
	QHostAddress			m_ipAddress;
	/**
	 * @brief UDP Tx Port
	 */
	quint16					m_udpTxPort;
	/**
	 * @brief UDP Rx Port
	 */
	quint16					m_udpRxPort;
	/**
	 * @brief TCP Port (Rx and Tx)
	 */
	quint16					m_tcpPort;
	/**
	 * @brief output enabled (can be overwriten by "forced" argument)
	 */
	bool					m_isEnabled;
	/**
	 * @brief m_useTcp is true if TCP should be used instead of UDP
	 */
	bool					m_useTcp;
	/**
	 * @brief Timer to connect again after error
	 */
	QTimer					m_tryConnectAgainTimer;
	/**
	 * @brief TCP frame mode (OSC 1.0 or 1.1 SLIP)
	 */
	OSCStream::EnumFrameMode m_tcpFrameMode;
	/**
	 * @brief log of incoming and / or outgoing messages
	 */
	mutable QStringList		m_log;
	/**
	 * @brief m_logIncomingMsg is true if incoming messages should be logged
	 */
	bool					m_logIncomingMsg;
	/**
	 * @brief m_logOutgoingMsg is true if outgoing messages should be logged
	 */
	bool					m_logOutgoingMsg;
	/**
	 * @brief m_eosUser is number of the Eos User (default = 0 -> Background User)
	 */
	QString					m_eosUser;
	/**
	 * @brief m_incompleteStreamData may contain the begin of an incomplete OSC packet
	 * (from TCP stream)
	 */
	QByteArray				m_incompleteStreamData;

    /**
     * @brief m_logChangedSignalDelay is a timer to delay the emission of the logChanged signal
     * to prevent the log being updated to often
     */
    mutable QTimer m_logChangedSignalDelay;
};

#endif // OSCWRAPPER_H
