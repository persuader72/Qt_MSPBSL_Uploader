#include "mymoteserialplugin.h"
#include "qextserialport.h"
#include <QtCore/qplugin.h>

QString MyMoteSerialPlugin::pluginName() {
    return QString("MyMoteSerialPlugin");
}

BootStrapLoader::bslState MyMoteSerialPlugin::timerTimeout(BootStrapLoader::bslState state) {
    //qDebug("MyMoteSerialPlugin::timerTimeout");
    switch(state) {
    default:
        break;
    }
    return state;
}

QByteArray MyMoteSerialPlugin::escapeOutput(const QByteArray &output) {
    int count;
    if((count=output.count('#'))==0) return output;
    else {
        QByteArray escaped = output;
        escaped.replace('#',QByteArray(2,'#'));
        return escaped;
    }
}

BootStrapLoader::bslState MyMoteSerialPlugin::incomingByte(quint8 byte) {
    Q_UNUSED(byte);
    qDebug("MyMoteSerialPlugin::incomingByte %02X %c",byte,byte);
    if(byte==117) return BootStrapLoader::serial; else return BootStrapLoader::afterConnect;
}

BootStrapLoader::bslState MyMoteSerialPlugin::afterSerialConnect() {
    qDebug("MyMoteSerialPlugin::afterSerialConnect");
    //QByteArray string("#u#u:05000615000100.#q");
    QByteArray string("#q#q");       //esce da serale, il comando è doppio per gestire qualunque stato
    string += ":05000611050100.";      //Master Uart 115200
    string += "#u";                  //Entro in UART
    string += "F0\n";                  //Mando Mote in BSL
    string += "#q";                  //esco dalla seriale
    string += ":05000615000100.";      //Imposta aux uart a 9600 su BSL
    string += "#u";                  //Entro in UART

    mSerialPort->write(string);

    return BootStrapLoader::serial;
}

BootStrapLoader::bslState MyMoteSerialPlugin::beforeSerialDisconnect() {
    return BootStrapLoader::idle;
}

Q_EXPORT_PLUGIN2(mymoteserialplugin, MyMoteSerialPlugin);
