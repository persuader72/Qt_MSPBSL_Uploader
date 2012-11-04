/****************************************************************************
** Copyright (c) 2012 Stefano Pagnottelli
** Web: http://github.com/persuader72/Qt_MSPBSL_Uploader
**
** This file is part of Qt_MSPBSL_Uploader.
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

#ifndef BSLPACKET_H
#define BSLPACKET_H

#include <QByteArray>

class BSLCoreMessage;
class BSLPacket {
public:
    enum eSequence { seqIdle,seqAckWait,seqHeaderWait,seqLenghtLWait,
                     seqLenghtHWait,seqReplyWait,seqCrcLWait,seqCrcHWait,seqDone,seqError };
public:
    BSLPacket();
    virtual ~BSLPacket();
public:
    eSequence sequence() { return mSequence; }
    void setSequence(eSequence seq) { mSequence=seq; }
    int timeout() const { return mTimeout; }
    bool incomingByte(quint8 incoming);
    bool hasReply() const { return mReply!=NULL && mReply->sequence()==seqDone; }
    const BSLCoreMessage *reply() const { return (const BSLCoreMessage *)mReply; }
    BSLCoreMessage *reply();
public:
    virtual const QByteArray assemblePacket();
    virtual void deassemblePacket(const QByteArray &);
private:
    quint16 payloadCrc();
    void crcAddByte(quint8 byte);
    quint16 crcAddByte(quint16 crc16,quint8 byte);
private:
    void clear();
private:
    eSequence mSequence;
    int mTimeout;
protected:
    QByteArray mPayload;
private:
    quint16 mLength;
    quint16 mCrc16;
private:
    BSLPacket *mReply;
};

#endif // BSLPACKET_H
