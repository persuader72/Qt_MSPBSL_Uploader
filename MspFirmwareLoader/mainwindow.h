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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qextserialenumerator.h"
#include "bootstraploader.h"
#include "qintelhexparser.h"
#include "serialplugininterface.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
protected:
    virtual void timerEvent(QTimerEvent *);
private slots:
    void onBslStateChanged(int state);
    void onBslErrorRised(const QString &,const QString &);
    void onBslReplyReceived(BSLPacket * packet);
private slots:
    void on_FirmwareLoadButton_clicked();
    void on_SerialConnectButton_clicked();
    void on_OperationStartButton_clicked();
    void on_SerialPluginCombo_currentIndexChanged(int index);

private:
    void loadPlugins();
    SerialPluginInterface *mSerialPlugin;
private:
    Ui::MainWindow *ui;
    BootStrapLoader *mBsl;
    QIntelHexParser parser;
    QextSerialEnumerator mSerialEnum;
private:
    QList<SerialPluginInterface *> mPlugins;
};

#endif // MAINWINDOW_H


