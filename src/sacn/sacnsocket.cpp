// Copyright 2016 Tom Barthel-Steer
// http://www.tomsteer.net
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "sacnsocket.h"

#include <QNetworkInterface>
#include <QNetworkAddressEntry>
#include <QDebug>
#include <QThread>
#include "ACNShare/ipaddr.h"
#include "streamcommon.h"


sACNRxSocket::sACNRxSocket(QObject *parent) : QUdpSocket(parent)
{

}

bool sACNRxSocket::bindMulticast(quint16 universe)
{
	bool ok = false;

    CIPAddr addr;
    GetUniverseAddress(universe, addr);

    // Bind to mutlicast address
    ok = bind(addr.ToQHostAddress(),
                   addr.GetIPPort(),
                   QAbstractSocket::ShareAddress | QAbstractSocket::ReuseAddressHint);

	// Join multicast on default NIC
    if (ok)
	{
		ok |= joinMulticastGroup(QHostAddress(addr.GetV4Address()));
    }

    if(ok)
	{
		qDebug() << "sACNRxSocket" << QThread::currentThreadId() << ": Joining Multicast Group:" << QHostAddress(addr.GetV4Address()).toString();
    }
    else
    {
        close();
		qDebug() << "sACNRxSocket" << QThread::currentThreadId() << ": Failed to bind RX socket";
    }

    return ok;
}

bool sACNRxSocket::bindUnicast()
{
    bool ok = false;

	// Bind to any address on default NIC
	ok = bind(STREAM_IP_PORT,
			  QAbstractSocket::ShareAddress | QAbstractSocket::ReuseAddressHint);

    if (!ok) {
        close();
		qDebug() << "sACNRxSocket" << QThread::currentThreadId() << ": Failed to bind RX socket";
    }

    return ok;
}

sACNTxSocket::sACNTxSocket(QObject *parent) : QUdpSocket(parent)
{

}

bool sACNTxSocket::bindMulticast()
{
    bool ok = false;

	// Bind to any address on default NIC
	ok = bind();
	setSocketOption(QAbstractSocket::MulticastLoopbackOption, QVariant(1));

    if(!ok)
		qDebug() << "sACNTxSocket" << QThread::currentThreadId() << ": Failed to bind TX socket";

    return ok;
}
