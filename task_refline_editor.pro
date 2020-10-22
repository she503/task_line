#-------------------------------------------------
#
# Project created by QtCreator 2020-10-20T21:25:11
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = task_refline_editor
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += \
    /opt/ros/melodic/include \
    /usr/include/eigen3 \
    /tergeo/include/calib2 \
    /tergeo/include/common2 \
    /tergeo/include/framework2 \
    /tergeo/include/hdmap2 \
    /tergeo/include/vehicle2

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    vehicle_item.cpp \
    editor_viewer.cpp \
    data_manager.cpp \
    map_manager.cpp \
    task_manager.cpp \
    ros_manager.cpp \
    refline_manager.cpp \
    task_control_frame.cpp \
    refline_edit_frame.cpp

HEADERS += \
        mainwindow.h \
    vehicle_item.h \
    editor_viewer.h \
    data_manager.h \
    map_manager.h \
    task_manager.h \
    ros_manager.h \
    utils.h \
    refline_manager.h \
    task_control_frame.h \
    refline_edit_frame.h

FORMS += \
        mainwindow.ui \
    task_control_frame.ui \
    refline_edit_frame.ui

RESOURCES += \
    icons.qrc
