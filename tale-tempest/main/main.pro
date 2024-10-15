TARGET = main

QT += core gui network widgets

CONFIG += c++11


# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

RESOURCES = main.qrc

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    utilities/qlog.cpp \
    utilities/qpy.cpp \
    tdialogconfig.cpp \
    tdialognewproject.cpp

HEADERS += \
    mainwindow.h \
    utilities/qlog.h \
    utilities/qpy.h \
    tdialogconfig.h \
    tdialognewproject.h

FORMS += \
    mainwindow.ui \
    tdialogconfig.ui \
    tdialognewproject.ui

TRANSLATIONS += \
    translations/zh_CN.ts

# use lrelease to handle translations
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
!isEmpty(target.path): INSTALLS += target

UI_DIR = $$PWD

# Additional runtime dependence
CONFIG(debug, debug|release): {
    QMAKE_POST_LINK += $$system(if not exist $$shell_path("$$OUT_PWD/debug/bearer") (mklink /D $$shell_path("$$OUT_PWD/debug/bearer") $$shell_path("$$PWD/../dist/bearer")))
    QMAKE_POST_LINK += $$system(if not exist $$shell_path("$$OUT_PWD/debug/iconengines") (mklink /D $$shell_path("$$OUT_PWD/debug/iconengines") $$shell_path("$$PWD/../dist/iconengines")))
    QMAKE_POST_LINK += $$system(if not exist $$shell_path("$$OUT_PWD/debug/imageformats") (mklink /D $$shell_path("$$OUT_PWD/debug/imageformats") $$shell_path("$$PWD/../dist/imageformats")))
    QMAKE_POST_LINK += $$system(if not exist $$shell_path("$$OUT_PWD/debug/platforms") (mklink /D $$shell_path("$$OUT_PWD/debug/platforms") $$shell_path("$$PWD/../dist/platforms")))
    QMAKE_POST_LINK += $$system(if not exist $$shell_path("$$OUT_PWD/debug/printsupport") (mklink /D $$shell_path("$$OUT_PWD/debug/printsupport") $$shell_path("$$PWD/../dist/printsupport")))
    QMAKE_POST_LINK += $$system(if not exist $$shell_path("$$OUT_PWD/debug/sensors") (mklink /D $$shell_path("$$OUT_PWD/debug/sensors") $$shell_path("$$PWD/../dist/sensors")))
    QMAKE_POST_LINK += $$system(if not exist $$shell_path("$$OUT_PWD/debug/sqldrivers") (mklink /D $$shell_path("$$OUT_PWD/debug/sqldrivers") $$shell_path("$$PWD/../dist/sqldrivers")))
    QMAKE_POST_LINK += $$system(if not exist $$shell_path("$$OUT_PWD/debug/styles") (mklink /D $$shell_path("$$OUT_PWD/debug/styles") $$shell_path("$$PWD/../dist/styles")))
    QMAKE_POST_LINK += $$system(if not exist $$shell_path("$$OUT_PWD/debug/webview") (mklink /D $$shell_path("$$OUT_PWD/debug/webview") $$shell_path("$$PWD/../dist/webview")))
}
