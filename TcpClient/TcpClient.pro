QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    book.cpp \
    friend.cpp \
    main.cpp \
    online.cpp \
    opewidget.cpp \
    privatechat.cpp \
    protocol.cpp \
    share.cpp \
    student_check_in.cpp \
    tcpclient.cpp

HEADERS += \
    book.h \
    friend.h \
    online.h \
    opewidget.h \
    privatechat.h \
    protocol.h \
    share.h \
    student_check_in.h \
    tcpclient.h

FORMS += \
    online.ui \
    privatechat.ui \
    share.ui \
    student_check_in.ui \
    tcpclient.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    FileType.qrc \
    config.qrc
