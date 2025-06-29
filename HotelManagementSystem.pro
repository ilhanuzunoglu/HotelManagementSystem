#-------------------------------------------------
#
# Project created by QtCreator 2025-06-19T14:40:41
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = HotelManagementSystem
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    src/admindashboard.cpp \
    src/database.cpp \
    src/loginform.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/receptionistdashboard.cpp \
    src/registerform.cpp \
    src/roomeditdialog.cpp \
    src/userdashboard.cpp \
    src/usereditdialog.cpp \
    src/invoicedialog.cpp

HEADERS += \
    src/admindashboard.h \
    src/database.h \
    src/loginform.h \
    src/mainwindow.h \
    src/receptionistdashboard.h \
    src/registerform.h \
    src/roomeditdialog.h \
    src/userdashboard.h \
    src/usereditdialog.h \
    src/invoicedialog.h

FORMS += \
    ui/mainwindow.ui \
    ui/loginform.ui \
    ui/registerform.ui \
    ui/userdashboard.ui \
    ui/receptionistdashboard.ui \
    ui/admindashboard.ui
