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
    manager.cpp \
    parser.cpp \
    job-creator.cpp \
    database-manager.cpp

HEADERS += \
    util.hpp \
    download-manager.hpp \
    manager.hpp \
    parser.hpp \
    job-creator.hpp \
    database-manager.hpp

DISTFILES += \
    conf.xml \
    database-organisation.xml

macx: LIBS += -L/usr/local/lib -lmongoclient -lboost_thread-mt -lboost_system -lboost_regex
macx: INCLUDEPATH += /usr/local/include

