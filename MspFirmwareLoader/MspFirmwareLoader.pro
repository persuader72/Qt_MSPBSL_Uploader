#-------------------------------------------------
#
# Project created by QtCreator 2012-10-21T08:54:52
#
#-------------------------------------------------

QT       += core gui

TARGET = MspFirmwareLoader
TEMPLATE = app

include(3rdparty/qextserialport/src/qextserialport.pri)

SOURCES += main.cpp\
    mainwindow.cpp \
    bootstraploader.cpp \
    bslpacket.cpp \
    qintelhexparser.cpp \
    bslcorecommmand.cpp \
    bslrxpassword.cpp \
    bslsendpacketevent.cpp \
    bslcoremessage.cpp \
    bslrxdatablock.cpp

HEADERS  += mainwindow.h \
    bootstraploader.h \
    bslpacket.h \
    qintelhexparser.h \
    bslcorecommmand.h \
    bslrxpassword.h \
    bslsendpacketevent.h \
    bslcoremessage.h \
    bslrxdatablock.h

FORMS    += mainwindow.ui
