#-------------------------------------------------
#
# Project created by QtCreator 2016-02-29T09:52:52
#
#-------------------------------------------------

QT += core gui widgets
QT += 3dcore 3drenderer 3dinput

TARGET = SubmarineSimulator
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    simulationview.cpp \
    simulation.cpp \
    submarine.cpp

HEADERS  += mainwindow.h \
    simulationview.h \
    simulation.h \
    submarine.h

FORMS    += mainwindow.ui
