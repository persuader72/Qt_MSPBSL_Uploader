TEMPLATE        = lib
CONFIG         += plugin
INCLUDEPATH    += ../../MspFirmwareLoader ../../MspFirmwareLoader/3rdparty/qextserialport/src
TARGET          = $$qtLibraryTarget(optoisolateduart)
DESTDIR         = ../../plugins

include(../../MspFirmwareLoader/3rdparty/qextserialport/src/qextserialport.pri)

HEADERS += \
    optoisolateduart.h

SOURCES += \
    optoisolateduart.cpp
