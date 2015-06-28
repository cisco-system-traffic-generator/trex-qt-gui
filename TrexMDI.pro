#-------------------------------------------------
#
# Project created by QtCreator 2014-10-12T13:19:13
#
#-------------------------------------------------

QT       += core gui webkitwidgets printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TrexViewer
TEMPLATE = app

RC_FILE = app_icon.rc

win32:CONFIG(release, debug|release): copydata.commands = copy $$shell_path($$PWD/external/zmq/dll/)libzmq.dll  $$shell_path($$OUT_PWD/release)
else:win32:CONFIG(debug, debug|release): copydata.commands = copy $$shell_path($$PWD/external/zmq/dll/)libzmq.dll  $$shell_path($$OUT_PWD/debug)
first.depends = $(first) copydata
export(first.depends)
export(copydata.commands)
QMAKE_EXTRA_TARGETS += first copydata
QMAKE_CXXFLAGS += /DNOMINMAX

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/external/zmq/lib_x64/Release/ -llibzmq
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/external/zmq/lib_x64/Debug/ -llibzmq
else:unix: LIBS += -L$$PWD/../zeromq-4.0.4/builds/msvc/ -llibzmq

INCLUDEPATH += $$PWD/external/zmq/include
DEPENDPATH += $$PWD/external/zmq/include

INCLUDEPATH += $$PWD/external/
DEPENDPATH += $$PWD/external/


SOURCES += main.cpp\
        mainwindow.cpp \
    cchild.cpp \
    QGlobalStats.cpp \
    zmq_thread.cpp \
    external/mustache.cpp \
    external/qcustomplot.cpp \
    child_global_win.cpp \
    latencywin.cpp \
    perportwin.cpp \
    zmq_gcontext.cpp \
    ledwidget.cpp \
    rx_check_win.cpp \
    histogramwin.cpp

HEADERS  += mainwindow.h \
    cchild.h \
    QGlobalStats.h \
    zmq_thread.h \
    external/mustache.h \
    external/qcustomplot.h \
    child_global_win.h \
    latencywin.h \
    perportwin.h \
    zmq_gcontext.h \
    ledwidget.h \
    rx_check_win.h \
    histogramwin.h

FORMS    += mainwindow.ui \
    child_global_win.ui \
    latencywin.ui \
    perportwin.ui \
    rx_check_win.ui \
    histogramwin.ui

RESOURCES += \
    trex.qrc

OTHER_FILES +=
