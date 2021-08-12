QT -= gui
QT += network websockets sql

CONFIG += c++11 console
CONFIG -= app_bundle

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
        app-net-manager.cpp \
        app-net-thread.cpp \
        app-web-thread.cpp \
        client-mgr.cpp \
        error.cpp \
        helper.cpp \
        load-server.cpp \
        local-socket.cpp \
        main.cpp \
        sql.cpp \
        zmienne.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    app-net-manager.h \
    app-net-thread.h \
    app-web-thread.h \
    client-mgr.h \
    error.h \
    helper.h \
    load-server.h \
    local-socket.h \
    sql.h \
    zmienne.h

DISTFILES += \
    config/load-server.conf
