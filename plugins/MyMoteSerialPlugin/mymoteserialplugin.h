#ifndef MYMOTESERIALPLUGIN_H
#define MYMOTESERIALPLUGIN_H

#include <QObject>
#include "serialplugininterface.h"

class MyMoteSerialPlugin : public QObject, SerialPluginInterface {
    Q_OBJECT
    Q_INTERFACES(SerialPluginInterface)
public:
    virtual QString pluginName();
    virtual bool hasEscapeOutput() { return true; }
    virtual QByteArray escapeOutput(const QByteArray &output);

    virtual BootStrapLoader::bslState incomingByte(quint8 byte);
    virtual BootStrapLoader::bslState afterSerialConnect();
    virtual BootStrapLoader::bslState beforeSerialDisconnect();
};

#endif // MYMOTESERIALPLUGIN_H

