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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qextserialport.h"

#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QProgressBar>
#include <QPluginLoader>

#include "bslsendpacketevent.h"
#include "bslrxpassword.h"
#include "bslcoremessage.h"
#include "bslrxpassword.h"
#include "bslrxdatablock.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),ui(new Ui::MainWindow) {
    ui->setupUi(this);

    loadPlugins();
    if(!mSerialPlugin) {
        qDebug("MainWindow::MainWindow Could not load the plugin");
    } else {
        qDebug("MainWindow::MainWindow Loaded Plugin %s",mSerialPlugin->pluginName().toAscii().constData());
    }

    QextPortInfo serinfo;
    QStringList result;
    QList<QextPortInfo> serials = QextSerialEnumerator::getPorts();
    foreach(serinfo,serials) result.append(serinfo.physName);

    result.sort();
    ui->SerialPortNameCombo->addItems(result);

    mBsl = new BootStrapLoader(this);
    if(mSerialPlugin) mBsl->setSerialPlugin(mSerialPlugin);

    connect(mBsl,SIGNAL(stateChanged(int)),this,SLOT(onBslStateChanged(int)));
    connect(mBsl,SIGNAL(errorRised(QString,QString)),SLOT(onBslErrorRised(QString,QString)));
    connect(mBsl,SIGNAL(replyReceived(BSLPacket*)),this,SLOT(onBslReplyReceived(BSLPacket*)));
    startTimer(333);
}

MainWindow::~MainWindow() {
    mBsl->quit();
    mBsl->wait();
    delete mBsl;
    delete ui;
}

void MainWindow::timerEvent(QTimerEvent *) {
}

void MainWindow::onBslStateChanged(int state) {
    qDebug("MainWindow::onBslStateChanged %d",state);
    switch(state) {
    case BootStrapLoader::idle:
        ui->SerialConnectButton->setText("Connect");
        ui->SerialPortGroup->setEnabled(true);
        ui->OperationGroup->setEnabled(false);
    case BootStrapLoader::serial:
        ui->SerialConnectButton->setText("Disconnect");
        ui->SerialPortGroup->setEnabled(true);
        break;
    case BootStrapLoader::bsl:
        ui->SerialPortGroup->setEnabled(true);
        if(parser.endOfFile()) ui->OperationGroup->setEnabled(true);
        break;
    case BootStrapLoader::working:
        ui->SerialPortGroup->setEnabled(false);
        ui->OperationGroup->setEnabled(false);
        break;
    }
}

void MainWindow::onBslErrorRised(const QString &title, const QString &text) {
    QMessageBox::warning(this,title,text);
}

void MainWindow::onBslReplyReceived(BSLPacket *packet) {
    qDebug("MainWindow::onBslReplyReceived");
    if(packet->reply()) {
        switch(packet->reply()->command()) {
        case BSLCoreMessage::Message:
            qDebug("MainWindow::onBslReplyReceived Message: %02X",packet->reply()->message());
            if(packet->extraData1()!=-1) {
                ui->OperationProgressBar->setValue(packet->extraData1());
            }
            break;
        case BSLCoreMessage::DataBlock:
            qDebug("MainWindow::onBslReplyReceived DataBlock: %s",packet->reply()->dataBlock().toHex().constData());
            break;
        }

    }
}

void MainWindow::on_FirmwareLoadButton_clicked() {
    QString fn=QFileDialog::getOpenFileName(this,"Select intel hex firmware file","","*.hex *.ihex");
    if(!fn.isNull()) {
        QFileInfo finfo(fn);

        QFile f(fn);
        ui->FirmwareSelectedFile->setText(fn);


        if(!f.open(QIODevice::ReadOnly)) {
            qDebug()<< f.errorString();
        } else {
            parser.parseFile(f);

            if(parser.endOfFile()) {
                ui->FirmwareFileLabel->setText(QString("File name: %1").arg(finfo.fileName()));
                ui->FirmwareSizeNameLabel->setText(QString("File size: %1 bytes").arg(finfo.size()));
                ui->FirmwareMemoryLabel->setText(QString("Memory: %1 bytes").arg(parser.sumTotalMemory()));
                ui->FirmwareBlocksLabel->setText(QString("Blocks: %1").arg(parser.segments().size()));
                ui->FirmwareAddrLabel->setText(QString("Address: 0x%1").arg(parser.segments().at(0).address,4,16,QChar('0')));
            }

            if(parser.endOfFile() && mBsl->state()==BootStrapLoader::bsl) {
                onBslStateChanged(mBsl->state());
            }
        }
    }
}

void MainWindow::on_SerialConnectButton_clicked() {
    if(mBsl->isRunning()) {
        mBsl->terminate();
        mBsl->wait();
    } else {
        mBsl->setPortName(ui->SerialPortNameCombo->currentText());
        mBsl->start();
    }
}

void MainWindow::on_OperationStartButton_clicked() {
    int blocks=2,block=0;
    foreach(const QIntelHexMemSegment &segment, parser.segments()) {
        blocks += segment.memory.size() / 32;
    }

    BSLPacket *pkt;
    // Intialize progress bar
    ui->OperationProgressBar->setValue(0);
    ui->OperationProgressBar->setMaximum(blocks);
    // Set BSL as working this will update UI status
    mBsl->setState(BootStrapLoader::working);
    // Bulk erase and unlink bsl
    pkt = new BSLCoreCommmand(BSLCoreCommmand::massErase,BSLCoreCommmand::NULL_ADDRESS);
    pkt->setExtraData(block++,0,0);
    mBsl->doPostPacket(pkt);
    pkt = new BSLRxPassword();
    pkt->setExtraData(block++,0,0);
    mBsl->doPostPacket(pkt);
    // Send memory segments as 32bytes blocks
    for(int i=0;i<parser.segments().count();i++) {
        int j=0;
        const QIntelHexMemSegment &segment = parser.segments().at(i);
        for(;j<segment.memory.size();j+=32) {
            pkt = new BSLRxDataBlock(segment.address+j,segment.memory.mid(j,32));
            pkt->setExtraData(block++,0,0);
            mBsl->doPostPacket(pkt);
        }
        qDebug() << j << segment.memory.size();
    }

}

void MainWindow::loadPlugins() {
    mSerialPlugin=NULL;

     QDir pluginsDir(qApp->applicationDirPath());
#if defined(Q_OS_WIN)
     if (pluginsDir.dirName().toLower() == "debug" || pluginsDir.dirName().toLower() == "release") pluginsDir.cdUp();
#endif
     pluginsDir.cdUp();
     pluginsDir.cd("plugins");
     foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = pluginLoader.instance();
        if(plugin) {
            mSerialPlugin = qobject_cast<SerialPluginInterface *>(plugin);
        }
     }
}
