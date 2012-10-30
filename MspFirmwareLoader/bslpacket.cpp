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
    // 100ms default timeout
    mTimeout=100;
    mReply=NULL;
    clear();
}

BSLPacket::~BSLPacket() {
    if(mReply) delete mReply;
}

bool BSLPacket::incomingByte(quint8 incoming) {
    switch(sequence()) {
    case seqAckWait:
        if(incoming==0x00) {
            setSequence(seqHeaderWait);
        } else {
            setSequence(seqError);
            qDebug("Wrong replay to data packet!!!");
        }
        break;
    case seqHeaderWait:
        if(incoming==0x80) {
            setSequence(seqLenghtLWait);
        } else {
            setSequence(seqError);
            qDebug("Wrong replay to data packet!!!");
        }
        break;
    case seqLenghtLWait:
        mLength = incoming;
        setSequence(seqLenghtHWait);
        break;
    case seqLenghtHWait:
        mLength = ((quint16)incoming)<<8;
        setSequence(seqReplyWait);
        break;
    case seqReplyWait:
        mPayload.resize(mPayload.size()+1);
        mPayload[mPayload.size()-1]=incoming;
        if(mPayload.size()==mLength) setSequence(seqCrcLWait);
        break;
    case seqCrcLWait:
        mCrc16 = incoming;
        setSequence(seqCrcHWait);
        break;
    case seqCrcHWait:
        mCrc16 = ((quint16)incoming)<<8;
        if(payloadCrc()==mCrc16) setSequence(seqDone); else setSequence(seqError);
        break;
    default:
        qDebug("Invalid sequence state!!!");
        break;
    }

    return mSequence==seqDone || mSequence==seqError;
}

BSLPacket *BSLPacket::reply() {
    if(!mReply) mReply=new BSLPacket();
    return mReply;
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

quint16 BSLPacket::payloadCrc() {
    quint16 crc16 = 0xFFFF;
    for(int i=0;i<mPayload.size();i++) crcAddByte(mPayload.at(i));
    return crc16;
}

void BSLPacket::crcAddByte(quint8 byte) {
    quint8 x = ((mCrc16>>8) ^ byte) & 0xff;
    x ^= x>>4;
    mCrc16 = (mCrc16 << 8) ^ (x << 12) ^ (x <<5) ^ x;
}

void BSLPacket::clear() {
    if(mReply) delete mReply;
    mSequence = seqIdle;
    mPayload.clear();
    mLength=0;
    mCrc16=0;
}
