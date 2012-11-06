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
#include "bslcorecommmand.h"
#include "bslcoremessage.h"
#include "serialplugininterface.h"
#include "qextserialport.h"

#include <QApplication>
#include <QDebug>

BootStrapLoader::BootStrapLoader(QObject *parent) : QThread(parent) {
    setState(idle);
    mSerialPortName="/dev/ttyUSB0";
    mSerialPort=NULL;
    // BSL Packt used to detect bsl (tx version)
    mPollPacket= new BSLCoreCommmand(0x19,BSLCoreCommmand::NULL_ADDRESS);
    mOutPacket=NULL;
    mSerialPlugin=NULL;
}

BootStrapLoader::~BootStrapLoader() {
    delete mPollPacket;
}

void BootStrapLoader::setState(BootStrapLoader::bslState state) {
    mState=state;
    emit stateChanged(mState);
}

void BootStrapLoader::setError(QString errorTitle, QString errorText) {
    emit errorRised(errorTitle,errorText);
}

void BootStrapLoader::doPostPacket(BSLPacket *packet) {
    QApplication::instance()->postEvent( this, new BslSendPacketEvent( packet ) );
}

void BootStrapLoader::doQueuePacket(BSLPacket *packet) {
    mOutQueue.append(packet);
    tryToSend();
}

void BootStrapLoader::run() {
    mTimer = new QTimer();
    mTimer->setInterval(50);
    connect(mTimer,SIGNAL(timeout()),this,SLOT(on_Timer_Timeout()));

    mSerialPort = new QextSerialPort();
    mSerialPort->setPortName(mSerialPortName);
    mSerialPort->setBaudRate(BAUD9600);
    connect(mSerialPort,SIGNAL(readyRead()),this,SLOT(on_SerialPort_ReadyRead()));


    if(mSerialPort->open(QIODevice::ReadWrite)) {
        mTimer->start(50);
        setState(serial);
        exec();
        mTimer->stop();
    } else {
        setError("Serial Port",QString("Can't open serial port %1 error %2").arg(mSerialPortName).arg(mSerialPort->errorString().toAscii().constData()));
    }

    qDebug("BootStrapLoader::run exited");
    if(mSerialPort->isOpen()) mSerialPort->close();
    delete mSerialPort;
    delete mTimer;
}

void BootStrapLoader::customEvent(QEvent *e) {
    if(e->type()==QEvent::User) {
        BslSendPacketEvent *ev=(BslSendPacketEvent *)e;
        mOutQueue.append(ev->packet());
        if(mOutPacket==NULL) tryToSend();
    }
}

void BootStrapLoader::on_SerialPort_ReadyRead() {
    quint8 incoming;
    while(mSerialPort->read((char *)&incoming,1)>0) {
        if(mOutPacket==NULL) {
            qDebug("Data (%d) recevived but no packet active!!!",incoming);
        } else {
            if(mOutPacket->reply()->incomingByte(incoming)) {
                mComplQueue.append(mOutPacket);

                qDebug() << mState << mOutPacket->reply()->sequence() <<  mOutPacket->hasReply();

                if(mState==serial && mOutPacket->hasReply()) {
                    BSLCoreCommmand* coreCommand = dynamic_cast<BSLCoreCommmand*>(mOutPacket);
                    if(coreCommand && coreCommand->command()==BSLCoreCommmand::txBslVersion) {
                        mOutPacket=NULL;
                        setState(bsl);
                        stateChanged(mState);
                    }
                } else {

                }

                if(mOutPacket) {
                    emit replyReceived(mOutPacket);
                    mOutPacket=NULL;
                    tryToSend();
                }
            }
        }
    }
}

void BootStrapLoader::on_Timer_Timeout() {
    switch(mState) {
    case serial:
        if(mOutPacket==NULL) {
            // Polling bootloader requesting version packet at 3Hz
            if(mTimeout.elapsed()>5000) {
                qDebug("BootStrapLoader::timerEvent");
                BSLPolling();
            }
        } else {
            if(mTimeout.elapsed()>mOutPacket->timeout()) {
               qDebug("BootStrapLoader::timerEvent timeout for packet");
               mOutPacket=NULL;
            }
        }
        break;
    case bsl:
        if(mOutPacket==NULL) {
            // TODO Polling in connected state ????
        } else {
            if(mTimeout.elapsed()>mOutPacket->timeout()) {
               qDebug("BootStrapLoader::timerEvent timeout for packet");
               mOutPacket=NULL;
               tryToSend();
            }
        }
        break;
    default:
        break;
    }
}

void BootStrapLoader::BSLPolling() {
    mPollPacket->setSequence(BSLPacket::seqIdle);
    mPollPacket->reply()->setSequence(BSLPacket::seqAckWait);
    mOutPacket=mPollPacket;
    mTimeout.start();
    tryToSend();
}

void BootStrapLoader::tryToSend() {
    if(mOutPacket==NULL && mOutQueue.size()>0) {
        mOutPacket = mOutQueue.takeFirst();
    }


    if(mOutPacket!=NULL) {
        qDebug("BootStrapLoader::tryToSend %d",mOutPacket->sequence());

        switch(mOutPacket->sequence()) {
        case BSLPacket::seqIdle:
            mTimeout.start();
            mOutPacket->setSequence(BSLPacket::seqAckWait);
            if(mSerialPlugin && mSerialPlugin->hasEscapeOutput())  {
                qDebug() << mSerialPlugin->escapeOutput(mOutPacket->assemblePacket().toHex());
                mSerialPort->write(mSerialPlugin->escapeOutput(mOutPacket->assemblePacket()));
            } else {
                qDebug() << mOutPacket->assemblePacket().toHex();
                mSerialPort->write(mOutPacket->assemblePacket());
            }
            break;
        default:
            qDebug("Packet already sent!!!");
            break;
        }
    } else {
        if(state()==working) setState(bsl);
    }
}

