TEMPLATE        = lib
CONFIG         += plugin
INCLUDEPATH    += ../../MspFirmwareLoader ../../MspFirmwareLoader/3rdparty/qextserialport/src
TARGET          = $$qtLibraryTarget(mymoteserialplugin)
DESTDIR         = ../../plugins

HEADERS += \
    mymoteserialplugin.h

SOURCES += \
    mymoteserialplugin.cpp
