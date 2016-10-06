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

#include <stdint.h>
#include <QtNetwork>
#include <algorithm>

#include "ArtNetSender.h"

int ARTNET_PORT = 6454;
int ARTNET_STRING_SIZE = 8;
char ARTNET_STRING[] = "Art-Net";
uint8_t ARTNET_VERSION = 14;

ArtNetSender::ArtNetSender()
    : udpSocket(this)
    , universes(16)
{
    for (uint i = 0; i < universes.size(); ++i) {
        universes[i] = ArtNetUniverse(0, 0, i);
    }
}

void ArtNetSender::sendData(std::vector<uint8_t> data) {
    uint8_t* message = universes[0].getPacketData(data);
    udpSocket.writeDatagram((char*)message, 18+512, QHostAddress::Broadcast, ARTNET_PORT);
}

void ArtNetSender::sendValue(int value) {
    std::vector<uint8_t> data(512);
    for (int i = 0; i < 512; i++) {
        data[i] = value;
    }
    sendData(data);
}


inline uint8_t lowByte(uint16_t value) {
    return uint8_t(value & 0xff);
}

inline uint8_t highByte(uint16_t value) {
    return uint8_t(value >> 8);
}

ArtNetUniverse::ArtNetUniverse(int net, int subnet, int universe) : message(18+512) {
    message[0] = 'A';
    message[1] = 'r';
    message[2] = 't';
    message[3] = '-';
    message[4] = 'N';
    message[5] = 'e';
    message[6] = 't';
    message[7] = 0;
    //OPCODE
    message[8] = 0x00; //OpCode Low Byte as per spec
    message[9] = 0x50; //OpCode High Byte as per spec
    //VERSION
    message[10] = 0; //ver High
    message[11] = 14; //ver Low
    //PHYSICAL PORT
    message[13] = 1;
    //ADDRESS
    auto address = getAddress(net, subnet, universe);
    message[14] = lowByte(address); //address low byte first
    message[15] = highByte(address); //address high byte
    //DMX LENGTH
    message[16] = highByte(512); //HI Byte
    message[17] = lowByte(512); //LOW Byte
}

uint16_t ArtNetUniverse::getAddress(int net, int subnet, int universe) {
    return uint16_t(net << 8 | subnet << 4 | universe);
}

uint8_t* ArtNetUniverse::getPacketData(std::vector<uint8_t> data) {
    //SEQUENCE (FRAME)
    message[12] = m_frame % 256; //current frame number
    //DMX DATA
    std::copy(data.begin(), data.end(), message.begin() + 18);
    //for (int i = 0; i < 512; i++) {
    //    message[18 + i] = data[i];
    //}
    m_frame++;
    return message.data();
}
