#include "OSCDiscovery.h"
#include "../osc/OSCParser.h"

#include <QtCore/QElapsedTimer>
#include <QtNetwork/QUdpSocket>
#include <QtNetwork/QNetworkInterface>

////////////////////////////////////////////////////////////////////////////////

bool OSCDiscovery::IsValidSendHostAddress(const QHostAddress &hostAddress)
{
	return (	hostAddress!=QHostAddress(QHostAddress::Null) &&
				hostAddress!=QHostAddress(QHostAddress::Broadcast) &&
				hostAddress!=QHostAddress(QHostAddress::LocalHostIPv6) &&
				hostAddress!=QHostAddress(QHostAddress::AnyIPv4) &&
				hostAddress!=QHostAddress(QHostAddress::AnyIPv6) &&
				hostAddress!=QHostAddress(QHostAddress::Any) );
}

////////////////////////////////////////////////////////////////////////////////

bool OSCDiscovery::IsValidRecvHostAddress(const QHostAddress &hostAddress)
{
	return (	hostAddress!=QHostAddress(QHostAddress::Null) &&
				hostAddress!=QHostAddress(QHostAddress::Broadcast) &&
				hostAddress!=QHostAddress(QHostAddress::AnyIPv4) &&
				hostAddress!=QHostAddress(QHostAddress::AnyIPv6) &&
				hostAddress!=QHostAddress(QHostAddress::Any) );
}

////////////////////////////////////////////////////////////////////////////////

bool OSCDiscoveryClient::sDiscoveryServer::operator<(const sDiscoveryServer &other) const
{
	int n = name.compare(other.name, Qt::CaseInsensitive);
	if(n != 0)
		return (n < 0);

	quint32 ip = hostAddress.toIPv4Address();
	quint32 otherIP = other.hostAddress.toIPv4Address();
	if(ip != otherIP)
		return (ip < otherIP);

	return (port < other.port);
}

////////////////////////////////////////////////////////////////////////////////

OSCDiscoveryClient::OSCDiscoveryClient(QObject* parent/* =0 */)
	: QThread(parent)
    , m_pClient(nullptr)
	, m_DiscoverServerPort(0)
	, m_DiscoveryClientPort(0)
	, m_Run(false)
{
}

////////////////////////////////////////////////////////////////////////////////

OSCDiscoveryClient::~OSCDiscoveryClient()
{
	Shutdown();
}

////////////////////////////////////////////////////////////////////////////////

void OSCDiscoveryClient::Initialize(Client &client, const QString &name, quint16 discoveryServerPort/* =OSCDiscovery::DISCOVERY_DEFAULT_SERVER_PORT */, quint16 discoveryClientPort/* =OSCDiscovery::DISCOVERY_DEFAULT_CLIENT_PORT */)
{
	if( !IsRunning() )
	{
		m_pClient = &client;
		m_Name = name;
		m_DiscoverServerPort = discoveryServerPort;
		m_DiscoveryClientPort = discoveryClientPort;
		m_Servers.clear();
		m_Run = true;
		start();
	}
}

////////////////////////////////////////////////////////////////////////////////

void OSCDiscoveryClient::Shutdown()
{
	m_Run = false;
	wait();
}

////////////////////////////////////////////////////////////////////////////////

bool OSCDiscoveryClient::SendRequest()
{
	bool success = false;

	OSCPacketWriter packet( OSCDiscovery::GetDiscoveryRequestPath() );
	packet.AddUInt32(m_DiscoveryClientPort);
	packet.AddString( m_Name.toUtf8().constData() );
	size_t bufBytes = 0;
	char *buf = packet.Create(bufBytes);
	if( buf )
	{
		QUdpSocket requestSocket;
        qint64 bytesSent = requestSocket.writeDatagram(buf, qint64(bufBytes), QHostAddress::Broadcast, m_DiscoverServerPort);
		if(bytesSent >= 0)
			success = true;
		else
			m_pClient->OSCDiscoveryClientClient_Log( QString("OSC discovery client writeDatagram(%1) failed with error %2").arg(m_DiscoverServerPort).arg(requestSocket.errorString()) );

		delete[] buf;
	}

	return success;
}

////////////////////////////////////////////////////////////////////////////////

bool OSCDiscoveryClient::GetServerInfoFromOSCPacket(char *buf, qint64 bytes, QString &serverName, quint16 &serverPort)
{
	bool success = false;

	size_t count = 2;
	OSCArgument *args = OSCArgument::GetArgs(buf, static_cast<size_t>(bytes), count);
	if( args )
	{
		if(count > 0)
		{
			uint32_t n = 0;
			if(args[0].GetUInt(n) && n<=0xffff)
			{
				serverPort = static_cast<quint16>(n);

				std::string str;
				if(count>1 && args[1].GetString(str))
					serverName = QString::fromUtf8( str.c_str() );
				else
					serverName.clear();

				success = true;
			}
		}

		delete[] args;
	}

	return success;
}

////////////////////////////////////////////////////////////////////////////////

void OSCDiscoveryClient::ProcessDatagram(const QHostAddress &hostAddress, char *buf, qint64 bytes)
{
    if(buf!=nullptr && bytes!=0 && OSCDiscovery::IsValidRecvHostAddress(hostAddress))
	{
		for(qint64 i=0; i<bytes; i++)
		{
			if(buf[i] == 0)
			{
				// found OSC path, is it a discovery reply?
				if(strcmp(buf,OSCDiscovery::GetDiscoveryReplyPath()) == 0)
				{
					sDiscoveryServer server;
					server.hostAddress = hostAddress;

					if( GetServerInfoFromOSCPacket(buf,bytes,server.name,server.port) )
					{
						m_pClient->OSCDiscoveryClientClient_Log( QString("OSC discovery client received reply from %1: %2 port %3").arg(server.name).arg(hostAddress.toString()).arg(server.port) );
					}
					else
					{
						m_pClient->OSCDiscoveryClientClient_Log( QString("OSC discovery client received reply from %1, but missing server port (using default instead)").arg(hostAddress.toString()) );
						server.name.clear();
						server.port = OSCDiscovery::DISCOVERY_DEFAULT_SERVER_PORT;
					}

					if(m_Servers.find(server) == m_Servers.end())
					{
						m_Servers.insert(server);
						m_pClient->OSCDiscoveryClientClient_Found(server);
					}
				}

				break;
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

void OSCDiscoveryClient::GetServerStringList(QStringList &strList) const
{
	strList.clear();

	if( !IsRunning() )
	{
		for(SERVERS::const_iterator serverIter=m_Servers.begin(); serverIter!=m_Servers.end(); serverIter++)
		{
			const sDiscoveryServer &server = *serverIter;
			QString serverDesc = QString("%1 @ %2 port %3")
				.arg( server.name )
				.arg( server.hostAddress.toString() )
				.arg( server.port );
			strList << serverDesc;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

void OSCDiscoveryClient::DebugPrintServers() const
{
	if( !IsRunning() )
	{
		QStringList strList;
		GetServerStringList(strList);

		qDebug() << "OSC discovery servers:";

		if( !strList.empty() )
		{
			unsigned int count = 0;
			for(QStringList::const_iterator strIter=strList.begin(); strIter!=strList.end(); strIter++)
				qDebug() << QString("%1. %2").arg(++count).arg(*strIter);
		}
		else
			qDebug() << "None found";
	}
	else
		qDebug() << "ERROR, OSCDiscoveryClient::DebugPrintServers while thread is running!";
}

////////////////////////////////////////////////////////////////////////////////

void OSCDiscoveryClient::run()
{
	QUdpSocket listenSocket;
	if( listenSocket.bind(m_DiscoveryClientPort) )
	{
		m_pClient->OSCDiscoveryClientClient_Log( QString("OSC discovery client listening on port %1").arg(m_DiscoveryClientPort) );

        char *buf = nullptr;
		qint64 bufBytes = 0;

		for(;;)
		{
			if( SendRequest() )
			{
				const qint64 REPLY_TIMEOUT = 5000;
				QElapsedTimer replyTimer;
				replyTimer.start();

				for(;;)
				{
					listenSocket.waitForReadyRead(50);

					while( m_Run )
					{
						qint64 datagramBytes = listenSocket.pendingDatagramSize();
						if(datagramBytes > 0)
						{
							if(buf && bufBytes<datagramBytes)
							{
								delete[] buf;
                                buf = nullptr;
							}

							if( !buf )
							{
								bufBytes = datagramBytes;
								buf = new char[ static_cast<size_t>(bufBytes) ];
							}

							QHostAddress hostAddress;
                            datagramBytes = listenSocket.readDatagram(buf, bufBytes, &hostAddress, /*port*/nullptr);
							if(datagramBytes >= 0)
								ProcessDatagram(QHostAddress(hostAddress.toIPv4Address()), buf, datagramBytes);
							else
								m_pClient->OSCDiscoveryClientClient_Log( QString("readDatagram failed with error %1").arg(listenSocket.errorString()) );
						}
						else if(datagramBytes == 0)
                            listenSocket.readDatagram(nullptr, 0, nullptr, nullptr);	// discard
						else
							break;

						msleep(1);
					}

					if(!m_Run || replyTimer.elapsed()>REPLY_TIMEOUT)
						break;

					msleep(10);
				}
			}

			if( !m_Run )
				break;

			msleep(100);
		}

		if( buf )
			delete[] buf;
	}
	else
		m_pClient->OSCDiscoveryClientClient_Log( QString("OSC discovery client listen bind(%1) failed with error %2").arg(m_DiscoveryClientPort).arg(listenSocket.errorString()) );
}

////////////////////////////////////////////////////////////////////////////////
