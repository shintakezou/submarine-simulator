#-------------------------------------------------
#
# Project created by QtCreator 2016-02-29T09:52:52
#
#-------------------------------------------------

QT       += core gui

QT += 3dcore 3drenderer 3dinput

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SubmarineSimulator
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    simulationview.cpp

HEADERS  += mainwindow.h \
    simulationview.h

FORMS    += mainwindow.ui
