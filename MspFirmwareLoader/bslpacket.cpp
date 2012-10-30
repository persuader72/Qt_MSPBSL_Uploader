/****************************************************************************
** Copyright (c) 2012 Stefano Pagnottelli
** Web: http://github.com/persuader72/Qt_MSPBSL_Uploader
**
** This file is part of Nome-Programma.
**
** Nome-Programma is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** Nome-Programma is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Nome-Programma.  If not, see <http://www.gnu.org/licenses/>.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
** NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
** LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
** OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
** WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**
****************************************************************************/

#include "bslpacket.h"

BSLPacket::BSLPacket()  {
    clear();
}

const QByteArray BSLPacket::assemblePacket() {
    QByteArray out(5+mPayload.size(),0);

    int i=0;
    out[i++]=0x80;
    quint16 length = mPayload.size();
    out[i++]=(quint8)(length&0x00FF);
    out[i++]=(quint8)((length&0xFF00)>>8);
    for(int j=0;j<mPayload.size();j++) out[i++]=mPayload.at(j);

    mCrc16=0xFFFF;
    for(int j=3;j<out.size()-2;j++) crcAddByte(out[j]);
    out[i++] = (quint8)(mCrc16&0x00FF);
    out[i++] = (quint8)((mCrc16&0xFF00)>>8);

    return out;
}

void BSLPacket::incomingByte(quint8 byte) {
    /*switch(mDecode) {
    case decodeHeader:
        if(byte==0x80) mDecode=decodeCommand;
        break;
    case decodeCommand:
        mCommand=byte;
        mDecode=decodeLength1;
        break;
    case decodeLength1:
        mLength=byte;
        mDecode=decodeLenght2;
        break;
    case decodeLenght2:
        if(mLength==byte) {
            mPayload.resize(mLength-4);
            mDecode=decodeAddressLo;
        } else {
            clear();
        }
        break;
    case decodeAddressLo:
        mAddress=byte;
        mDecode=decodeAddressHi;
        break;
    case decodeAddressHi:
        mAddress=byte<<8;
        mDecode=decodeDataByteLo;
        break;
    case decodeDataByteLo:
        mDataBytes=byte;
        mDecode=decocdeDataByteHi;
        break;
    case decocdeDataByteHi:
        mDataBytes=byte<<8;
        mDecode=decodePayload;
        break;
    case decodePayload:
        mPayload[0]=byte;
        break;
    case decodeCrcLo:
        mPayload[0]=byte;
        break;
    case decocdeCrcHi:
        mPayload[0]=byte;
        break;
    }*/
}

void BSLPacket::crcAddByte(quint8 byte) {
    quint8 x = ((mCrc16>>8) ^ byte) & 0xff;
    x ^= x>>4;
    mCrc16 = (mCrc16 << 8) ^ (x << 12) ^ (x <<5) ^ x;
}

void BSLPacket::clear() {
    mSequence = seqIdle;

    mPayload.clear();
    mCommandReply=false;
    mLength=0;
    mCrc16=0;
}
