#-------------------------------------------------
#
# Project created by QtCreator 2016-02-29T09:52:52
#
#-------------------------------------------------

QT += core gui widgets printsupport
QT += 3dcore 3drenderer 3dinput

QT_CONFIG -= no-pkg-config
CONFIG += link_pkgconfig
PKGCONFIG += bullet

CONFIG += c++11

TARGET = SubmarineSimulator
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    simulation.cpp \
    submarine.cpp \
    fluid.cpp \
    forcearrow.cpp \
    simulationpropertiesdialogue.cpp \
    qcustomplot.cpp \
    fin.cpp \
    physics/force.cpp \
    physics/torque.cpp

HEADERS  += mainwindow.h \
    simulation.h \
    submarine.h \
    fluid.h \
    forcearrow.h \
    simulationpropertiesdialogue.h \
    qcustomplot.h \
    fin.h \
    physics/force.h \
    physics/torque.h

FORMS    += mainwindow.ui \
    simulationpropertiesdialogue.ui

RESOURCES += \
    resources.qrc

mac {
    PKG_CONFIG = /usr/local/bin/pkg-config
}
