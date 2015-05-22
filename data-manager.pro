#-------------------------------------------------
#
# Project created by QtCreator 2015-05-19T20:32:00
#
#-------------------------------------------------

QT       += core network

QT       -= gui

TARGET = data-manager
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    util.cpp \
    download-manager.cpp \
    manager.cpp

HEADERS += \
    util.hpp \
    download-manager.hpp \
    manager.hpp

DISTFILES += \
    conf.xml \
    database-organisation.xml
