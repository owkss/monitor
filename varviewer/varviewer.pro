# База
QT += core gui widgets network

TARGET = varviewer
TEMPLATE = app

CONFIG += c++11
DEFINES += QT_DEPRECATED_WARNINGS

# Дата сборки
win32: COMPILE_DATE = $$system(date /T)
unix: COMPILE_DATE = $$system(date +%d.%m.%Y)
BUILD_DATE = $$sprintf("%1.%2.%3", $$section(COMPILE_DATE, ., 2, 2), $$section(COMPILE_DATE, ., 1, 1), $$section(COMPILE_DATE, ., 0, 0))
DEFINES += VARVIEWERBUILDDATE=\\\"$$BUILD_DATE\\\"

# Версия
VERSION = 1.0.1.0
VER_MAJ = $$section(VERSION, ., 0, 0)
VER_MIN = $$section(VERSION, ., 1, 1)
VER_PAT = $$section(VERSION, ., 2, 2)
PROJECT_VERSION_MAJOR = $$VER_MAJ
PROJECT_VERSION_MINOR = $$VER_MIN
PROJECT_VERSION_PATCH = $$VER_PAT
PROJECT_VERSION_TWEAK = $$section(VERSION, ., 3, 3)

# Директория сборки
CONFIG(release, debug|release){
    DESTDIR = build/release
    MOC_DIR = build/release/moc
    OBJECTS_DIR = build/release/obj
    DEFINES += QT_NO_DEBUG_OUTPUT
} else {
    DESTDIR = build/debug
    MOC_DIR = build/debug/moc
    OBJECTS_DIR = build/debug/obj
} # CONFIG

# Файлы и пути
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

RESOURCES += \
    images.qrc
