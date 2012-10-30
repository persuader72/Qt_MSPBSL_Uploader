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

#include "bootstraploader.h"
#include "bslsendpacketevent.h"

#include "qextserialport.h"

#include <QDebug>

BootStrapLoader::BootStrapLoader(QObject *parent) : QThread(parent) {
    setState(idle);
    mSerialPortName="/dev/ttyUSB0";
    mSerialPort=NULL;
}

BootStrapLoader::~BootStrapLoader() {
}

void BootStrapLoader::setState(BootStrapLoader::bslState state) {
    mState=state;
    emit onStateChanged(mState);
}

void BootStrapLoader::setError(QString errorTitle, QString errorText) {
    emit onErrorRised(errorTitle,errorText);
}

void BootStrapLoader::doQueuePacket(BSLPacket *packet) {
    mOutQueue.append(packet);
    tryToSend();
}

void BootStrapLoader::run() {
    mSerialPort = new QextSerialPort();
    mSerialPort->setPortName(mSerialPortName);
    mSerialPort->setBaudRate(BAUD115200);
    connect(mSerialPort,SIGNAL(readyRead()),this,SLOT(on_SerialPort_ReadyRead()));

    if(mSerialPort->open(QIODevice::ReadWrite)) {
        setState(serial);
        exec();
    } else {
        setError("Serial Port",QString("Can't open serial port %1").arg(mSerialPortName));
    }

    qDebug("BootStrapLoader::run exited");
    if(mSerialPort->isOpen()) mSerialPort->close();
    delete mSerialPort;
}

void BootStrapLoader::customEvent(QEvent *e) {
    if(e->type()==QEvent::User) {
        BslSendPacketEvent *ev=(BslSendPacketEvent *)e;
        mOutQueue.append(ev->packet());
        tryToSend();
    }
}

void BootStrapLoader::on_SerialPort_ReadyRead() {
    quint8 incoming;
    while(mSerialPort->read((char *)&incoming,1)>0) {
        if(mOutPacket==NULL) {
            qDebug("Data recevived but no packet active!!!");
        } else {
            switch(mOutPacket->sequence()) {
            case BSLPacket::seqDataAckWait:
                if(incoming==0x90) {
                    if(mOutPacket->commandReply()) {
                        mOutPacket->setSequence(BSLPacket::seqReplyWait);
                    } else {
                        mOutPacket->setSequence(BSLPacket::seqIdle);
                        mComplQueue.append(mOutPacket);
                        mOutPacket = NULL;
                        tryToSend();
                    }
                } else {
                    mOutPacket->setSequence(BSLPacket::seqError);
                    qDebug("Wrong replay to data packet!!!");
                }
                break;
            default:
                qDebug("Packet already sent!!!");
                break;
            }
        }
    }
}

void BootStrapLoader::tryToSend() {
    if(mOutPacket==NULL && mOutQueue.size()>0) {
        mOutPacket = mOutQueue.takeFirst();
    }


    if(mOutPacket!=NULL) {
        qDebug("BootStrapLoader::tryToSend %d",mOutPacket->sequence());

        switch(mOutPacket->sequence()) {
        case BSLPacket::seqIdle:
            //mOutPacket->setSequence(BSLPacket::seqDataAckWait);
            //qDebug() << mOutPacket->assemblePacket().toHex();
            //mSerialPort->write(mOutPacket->assemblePacket());
            break;
        default:
            qDebug("Packet already sent!!!");
            break;
        }
    }
}
