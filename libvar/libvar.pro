CONFIG(release, debug|release){
    DESTDIR = release
    MOC_DIR = release/moc
    OBJECTS_DIR = release/obj
    DEFINES += QT_NO_DEBUG_OUTPUT
} else {
    DESTDIR = debug
    MOC_DIR = debug/moc
    OBJECTS_DIR = debug/obj
}

QT += core
TARGET = var
CONFIG += c++11
TEMPLATE = lib
DEFINES += VAR_LIBRARY

SOURCES += \
    var.cpp \
    var_p.cpp

HEADERS += \
    var.h \
    var_p.h \
    variable.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
