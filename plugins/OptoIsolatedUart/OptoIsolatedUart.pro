TEMPLATE        = lib
CONFIG         += plugin
INCLUDEPATH    += ../../MspFirmwareLoader ../../MspFirmwareLoader/3rdparty/qextserialport/src
TARGET          = $$qtLibraryTarget(optoisolateduart)
DESTDIR         = ../../plugins

HEADERS += \
    optoisolateduart.h \
    optoisolateduart.h

SOURCES += \
    optoisolateduart.cpp
