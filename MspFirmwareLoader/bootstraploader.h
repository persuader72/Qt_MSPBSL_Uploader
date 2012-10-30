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

#ifndef BOOTSTRAPLOADER_H
#define BOOTSTRAPLOADER_H

#include <QThread>
#include "bslpacket.h"

class QextSerialPort;
class BootStrapLoader : public QThread {
    Q_OBJECT
public:
    enum bslState { idle,serial,bsl,working };
    explicit BootStrapLoader(QObject *parent = 0);
    virtual ~BootStrapLoader();
public:
    bslState state() { return mState; }
    void setState(bslState state);
    void setError(QString errorTitle,QString errorText);
    void setPortName(const QString &portname) { mSerialPortName=portname; }
public:
    void doQueuePacket(BSLPacket *packet);
protected:
    virtual void run();
    virtual void customEvent(QEvent * e);
private slots:
    void on_SerialPort_ReadyRead();
private:
    void tryToSend();
private:
    bslState mState;
private:
    int mIncomingPacketPosition;
    QString mSerialPortName;
    QextSerialPort *mSerialPort;
    QByteArray mIncomingPacket;
    //BSLPacket mIncomingPacket;
private:
    BSLPacket *mOutPacket;
    QList<BSLPacket *> mOutQueue;
    QList<BSLPacket *> mComplQueue;
signals:
    void onStateChanged(int state);
    void onErrorRised(const QString &title,const QString &text);
};

#endif // BOOTSTRAPLOADER_H
