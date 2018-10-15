#pragma once
#ifndef OSC_DISCOVERY_H
#define OSC_DISCOVERY_H

#include <QtCore/QThread>
#include <QtNetwork/QHostAddress>

#include <set>

class QUdpSocket;

////////////////////////////////////////////////////////////////////////////////
//
// [-- OSCDiscoveryClient --]
//
// (typically the remote device, wanting to discover consoles)
//
// 1. Start listening for unicast udp packets on DISCOVERY_DEFAULT_CLIENT_PORT (or specified)
// 2. Broadcast discovery request, OSC Packet: "/etc/discovery/request", <uint32: my listen port (DISCOVERY_DEFAULT_CLIENT_PORT)>, <string: my name>
// 3. Collect discovery replies for 5 seconds, OSC Packet: "/etc/discovery/reply", <uint32: server port (DISCOVERY_DEFAULT_SERVER_PORT)>, <string: server name>
//     a. Callback upon discovery OSCDiscoveryClientClient_Found(...)
// 4. Repeat steps [2-3] three times
// 5. Thread finishes, app may now call GetServers() for a complete list
//
////////////////////////////////////////////////////////////////////////////////
//
// [-- OSCDiscoveryServer --]
//
// (typically the console, replying to remote device discovery requests)
//
// 1. Listen for broadcast udp packets on DISCOVERY_DEFAULT_SERVER_PORT (or specified)
// 2. Collect discovery requests, OSC Packet: "/etc/discovery/request", <uint32: client port (DISCOVERY_DEFAULT_CLIENT_PORT)>, <string: client name>
// 3. Send unicast discovery reply to all requests, OSC Packet: "/etc/discovery/reply", <uint32: my OSC tcp service port (OSCTcpServer::DEFAULT_PORT)>, <string: my name>
//
////////////////////////////////////////////////////////////////////////////////

class OSCDiscovery
{
public:
	enum EnumConstants
	{
		DISCOVERY_DEFAULT_SERVER_PORT	= 3034,
		DISCOVERY_DEFAULT_CLIENT_PORT	= 3035
	};

	static bool IsValidSendHostAddress(const QHostAddress &hostAddress);
	static bool IsValidRecvHostAddress(const QHostAddress &hostAddress);
	static const char* GetDiscoveryRequestPath() {return "/etc/discovery/request";}
	static const char* GetDiscoveryReplyPath() {return "/etc/discovery/reply";}
};

////////////////////////////////////////////////////////////////////////////////

class OSCDiscoveryClient
	: private QThread
{
public:
	struct sDiscoveryServer
	{
		sDiscoveryServer()
			: port(0)
		{}

		bool operator<(const sDiscoveryServer &other) const;

		QString			name;
		QHostAddress	hostAddress;
		quint16			port;
	};

	typedef std::set<sDiscoveryServer> SERVERS;

	class Client
	{
	public:
        virtual ~Client() = default;
		virtual void OSCDiscoveryClientClient_Log(const QString &text) = 0;
		virtual void OSCDiscoveryClientClient_Found(const sDiscoveryServer &server) = 0;
	};

    OSCDiscoveryClient(QObject *parent=nullptr);
	~OSCDiscoveryClient() override;

	virtual void Initialize(Client &client, const QString &name, quint16 discoveryServerPort=OSCDiscovery::DISCOVERY_DEFAULT_SERVER_PORT, quint16 discoveryClientPort=OSCDiscovery::DISCOVERY_DEFAULT_CLIENT_PORT);
	virtual void Shutdown();
	virtual bool IsRunning() const {return isRunning();}

	// only for use while thead is not running
    virtual const SERVERS* GetServers() const {return (IsRunning() ? nullptr : (&m_Servers));}
	virtual void GetServerStringList(QStringList &strList) const;
	virtual void DebugPrintServers() const;

protected:
	Client	*m_pClient;
	QString	m_Name;
	quint16	m_DiscoverServerPort;
	quint16	m_DiscoveryClientPort;
	SERVERS	m_Servers;
	bool	m_Run;

	void run() override;
	bool SendRequest();
	void ProcessDatagram(const QHostAddress &hostAddress, char *buf, qint64 bytes);

	static bool GetServerInfoFromOSCPacket(char *buf, qint64 bytes, QString &serverName, quint16 &serverPort);
};

////////////////////////////////////////////////////////////////////////////////

#endif
