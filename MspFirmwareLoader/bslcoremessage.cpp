#include "bslcoremessage.h"

BSLCoreMessage::BSLCoreMessage() {
    setSequence(seqAckWait);
}

void BSLCoreMessage::deassemblePacket(const QByteArray &payload) {
    if(mPayload.size()==0) {
        setSequence(seqError);
        return;
    }

    mCommand=payload.at(0);
    if(mPayload.size()>1) mMessage=mPayload.right(mPayload.size()-1);
    setSequence(seqDone);
}
