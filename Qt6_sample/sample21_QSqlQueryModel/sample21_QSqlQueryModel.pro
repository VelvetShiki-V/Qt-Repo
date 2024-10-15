QT       += core gui
QT       += sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    filenamefilesystemmodel.cpp \
    main.cpp \
    mainwindow.cpp \
    tdialogaccessdata.cpp \
    tdialogdomaindata.cpp \
    tdialogemployeedata.cpp \
    tdialoglogin.cpp \
    tdialogrbacdata.cpp \
    tdialogroledata.cpp \
    tdialoguserdata.cpp

HEADERS += \
    filenamefilesystemmodel.h \
    mainwindow.h \
    tdialogaccessdata.h \
    tdialogdomaindata.h \
    tdialogemployeedata.h \
    tdialoglogin.h \
    tdialogrbacdata.h \
    tdialogroledata.h \
    tdialoguserdata.h

FORMS += \
    mainwindow.ui \
    tdialogaccessdata.ui \
    tdialogdomaindata.ui \
    tdialogemployeedata.ui \
    tdialoglogin.ui \
    tdialogrbacdata.ui \
    tdialogroledata.ui \
    tdialoguserdata.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    icons.qrc
