QT += core gui widgets network

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

win32 {
    LIBS +=
}

CONFIG += c++11
TEMPLATE = app
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    application.cpp \
    delegate.cpp \
    executable.cpp \
    main.cpp \
    mainwindow.cpp \
    varview.cpp

HEADERS += \
    application.h \
    delegate.h \
    executable.h \
    mainwindow.h \
    variable.h \
    varview.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    images.qrc
