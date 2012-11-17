#include "optoisolateduart.h"
#include "qextserialport.h"
#include <QtCore/qplugin.h>

QString OptoIsolaredUart::pluginName() {
    return QString("OptoIsolaredUart");
}

BootStrapLoader::bslState OptoIsolaredUart::timerTimeout(BootStrapLoader::bslState state) {
    return state;
}

QByteArray OptoIsolaredUart::escapeOutput(const QByteArray &output) {
    return output;
}

BootStrapLoader::bslState OptoIsolaredUart::incomingByte(quint8 byte) {
    Q_UNUSED(byte);
    return BootStrapLoader::bsl;
}

BootStrapLoader::bslState OptoIsolaredUart::afterSerialConnect() {
    mSerialPort->setDtr(false);
    mSerialPort->setRts(true);
    return BootStrapLoader::bsl;
}

BootStrapLoader::bslState OptoIsolaredUart::beforeSerialDisconnect() {
    return BootStrapLoader::idle;
}

Q_EXPORT_PLUGIN2(optoisolareduart, OptoIsolaredUart);
