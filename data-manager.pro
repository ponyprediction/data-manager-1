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
    training-set-creator.cpp \
    solver/solver.cpp \
    intelligence/brain.cpp \
    intelligence/neuron.cpp \
    intelligence/neuron-blueprint.cpp

HEADERS += \
    util.hpp \
    download-manager.hpp \
    manager.hpp \
    parser.hpp \
    database-manager.hpp \
    VT100.h \
    training-set-creator.hpp \
    solver/solver.hpp \
    intelligence/brain.hpp \
    intelligence/neuron.hpp \
    intelligence/neuron-blueprint.hpp

DISTFILES += \
    database-organisation.xml \
    manual \
    data-manager.conf \
    scripts/reset-predictions.js \
    scripts/reset-races.sh \
    scripts/reset-predictions.sh \
    scripts/reset-races.js

macx: LIBS += -L/usr/local/lib -lmongoclient -lboost_thread-mt -lboost_system -lboost_regex
macx: INCLUDEPATH += /usr/local/include

unix:!macx: LIBS += -L/home/loic/mongo-client-driver/lib/ -lmongoclient -lmongoclient -lboost_thread -lboost_system -lboost_regex
unix:!macx: INCLUDEPATH += /home/loic/mongo-client-driver/include
unix:!macx: DEPENDPATH += /home/loic/mongo-client-driver/include
unix:!macx: PRE_TARGETDEPS += /home/loic/mongo-client-driver/lib/libmongoclient.a
