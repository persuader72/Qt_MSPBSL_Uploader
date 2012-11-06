TEMPLATE        = lib
CONFIG         += plugin
INCLUDEPATH    += ../../MspFirmwareLoader
TARGET          = $$qtLibraryTarget(mymoteserialplugin)
DESTDIR         = ../../plugins

HEADERS += \
    mymoteserialplugin.h

SOURCES += \
    mymoteserialplugin.cpp
