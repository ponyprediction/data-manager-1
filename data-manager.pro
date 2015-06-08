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
    database-manager.cpp \
    training-set-creator.cpp

HEADERS += \
    util.hpp \
    download-manager.hpp \
    manager.hpp \
    parser.hpp \
    database-manager.hpp \
    VT100.h \
    training-set-creator.hpp

DISTFILES += \
    database-organisation.xml \
    manual \
    data-manager.conf

macx: LIBS += -L/usr/local/lib -lmongoclient -lboost_thread-mt -lboost_system -lboost_regex
macx: INCLUDEPATH += /usr/local/include

unix:!macx: LIBS += -L/home/loic/mongo-client-driver/lib/ -lmongoclient -lmongoclient -lboost_thread -lboost_system -lboost_regex
unix:!macx: INCLUDEPATH += /home/loic/mongo-client-driver/include
unix:!macx: DEPENDPATH += /home/loic/mongo-client-driver/include
unix:!macx: PRE_TARGETDEPS += /home/loic/mongo-client-driver/lib/libmongoclient.a
