#ifndef SERIALPLUGININTERFACE_H
#define SERIALPLUGININTERFACE_H
#include <QString>
#include "bootstraploader.h"

class SerialPluginInterface {
public:
    virtual ~SerialPluginInterface() {}
    virtual QString pluginName() = 0;
    virtual void setSerialPort(QextSerialPort *serialport) = 0;
    virtual BootStrapLoader::bslState timerTimeout(BootStrapLoader::bslState state) =0;

    virtual bool hasEscapeOutput() = 0;
    virtual QByteArray escapeOutput(const QByteArray &output) = 0;

    virtual BootStrapLoader::bslState incomingByte(quint8 byte) = 0;
    virtual BootStrapLoader::bslState afterSerialConnect() = 0;
    virtual BootStrapLoader::bslState beforeSerialDisconnect() = 0;
};

Q_DECLARE_INTERFACE(SerialPluginInterface,
                    "com.siralab.Msp430Bsl.SerialPluginInterface/1.0")

#endif // SERIALPLUGININTERFACE_H
