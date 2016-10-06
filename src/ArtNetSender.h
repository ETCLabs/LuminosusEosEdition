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

#ifndef ARTNETSENDER_H
#define ARTNETSENDER_H

#include <QObject>
#include <QUdpSocket>

#include <stdint.h>


struct ArtNetUniverse {
    std::vector<uint8_t> message;
    long m_frame = 0;
    ArtNetUniverse() {}
    ArtNetUniverse(int net, int subnet, int universe);
    uint16_t getAddress(int net, int subnet, int universe);
    uint8_t* getPacketData(std::vector<uint8_t> data);
};

class ArtNetSender : public QObject
{
    Q_OBJECT

public:
    ArtNetSender();

public slots:
    void sendData(std::vector<uint8_t> data);
    void sendValue(int value);

private:
    QUdpSocket udpSocket;
    std::vector<ArtNetUniverse> universes;
};

#endif // ARTNETSENDER_H

