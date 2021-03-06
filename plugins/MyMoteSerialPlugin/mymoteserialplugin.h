#ifndef MYMOTESERIALPLUGIN_H
#define MYMOTESERIALPLUGIN_H

#include <QObject>
#include "serialplugininterface.h"

class MyMoteSerialPlugin : public QObject, SerialPluginInterface {
    Q_OBJECT
    Q_INTERFACES(SerialPluginInterface)
public:
    MyMoteSerialPlugin(QObject * parent = 0) : QObject(parent) , mSerialPort(0) {}
public:
    virtual QString pluginName();
    virtual void setSerialPort(QextSerialPort *serialport) { mSerialPort=serialport; }
    virtual BootStrapLoader::bslState timerTimeout(BootStrapLoader::bslState state);

    virtual bool hasEscapeOutput() { return true; }
    virtual QByteArray escapeOutput(const QByteArray &output);

    virtual BootStrapLoader::bslState incomingByte(quint8 byte);
    virtual BootStrapLoader::bslState afterSerialConnect();
    virtual BootStrapLoader::bslState beforeSerialDisconnect();
private:
    QextSerialPort *mSerialPort;
};

#endif // MYMOTESERIALPLUGIN_H

