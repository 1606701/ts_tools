QT       += sql 
QT 	     += concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    basic_info.cpp \
    database.cpp \
    dialog.cpp \
    epg.cpp \
    main.cpp \
    mainwindow.cpp \
    my_thread.cpp \
    section_data.cpp \
    task.cpp

HEADERS += \
    basic_info.h \
    database.h \
    dialog.h \
    epg.h \
    mainwindow.h \
    my_thread.h \
    section_data.h \
    section_data.h \
    task.h

FORMS += \
    basic_info.ui \
    dialog.ui \
    epg.ui \
    mainwindow.ui \
    section_data.ui

TRANSLATIONS += \
    TS_tools_zh_CN.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
RC_FILE = TS_tools.rc
