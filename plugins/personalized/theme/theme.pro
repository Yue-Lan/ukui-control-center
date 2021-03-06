#-------------------------------------------------
#
# Project created by QtCreator 2019-06-21T10:50:19
#
#-------------------------------------------------
QT       += widgets KConfigCore KI18n x11extras

TEMPLATE = lib
CONFIG += plugin


TARGET = $$qtLibraryTarget(theme)
DESTDIR = ../../../pluginlibs


include(../../../env.pri)
include($$PROJECT_COMPONENTSOURCE/switchbutton.pri)

INCLUDEPATH   +=  \
                 $$PROJECT_COMPONENTSOURCE \
                 $$PROJECT_ROOTDIR \

LIBS          += -L/usr/lib/ -lgsettings-qt -lX11 -lXfixes -lXcursor


#DEFINES += QT_DEPRECATED_WARNINGS


SOURCES += \
    cursor/cursortheme.cpp \
    cursor/xcursortheme.cpp \
        theme.cpp \
    themewidget.cpp \
    widgetgroup.cpp

HEADERS += \
    cursor/config-X11.h \
    cursor/cursortheme.h \
    cursor/xcursortheme.h \
        theme.h \
    themewidget.h \
    widgetgroup.h

FORMS += \
        theme.ui

RESOURCES +=
