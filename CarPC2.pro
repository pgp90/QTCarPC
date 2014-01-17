#-------------------------------------------------
#
# Project created by QtCreator 2014-01-16T14:52:43
#
#-------------------------------------------------

#QT       += core gui
QT += core gui \
      network \
      xml \
      multimedia \
      multimediawidgets \
      widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CarPC2
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp
#    audio.cpp \
#    audio_mac.cpp

HEADERS  += mainwindow.h
# \
#    audio.h \
#    audio_mac.h

win32 {
    SOURCES += audio_win.cpp
    HEADERS += audio_win.h
}
linux {
    SOURCES += audio_linux.cpp
    HEADERS += audio_linux.h
}
macx {
    SOURCES += audio_mac.cpp
    HEADERS += audio_mac.h
}
FORMS    += mainwindow.ui
