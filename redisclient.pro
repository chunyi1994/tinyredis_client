TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    tcpclient.cc

HEADERS += \
    tcpclient.h \
    utils.h

