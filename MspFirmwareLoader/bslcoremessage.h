#ifndef BSLCOREMESSAGE_H
#define BSLCOREMESSAGE_H

#include "bslpacket.h"

class BSLCoreMessage : public BSLPacket {
public:
    BSLCoreMessage();
    quint8 command() const { return mCommand; }
    const QByteArray &message() const { return mMessage; }
protected:
    virtual void deassemblePacket(const QByteArray &payload);
private:
    quint8 mCommand;
    QByteArray mMessage;
};

#endif // BSLCOREMESSAGE_H
