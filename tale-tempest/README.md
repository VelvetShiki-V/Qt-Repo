# QTemplate

The QTemplate project provides a generic Qt project template.

We emphasize the project that works well with the Qt 5.15.2 and MSVC-2019. QtTemplate
 is intended to be widely useful in Qt desktup application development.

The following Environment Variables on host machine are required (given that MSVC 2019 [142] is installed and Qt's installation directory is "C:\Qt5"):

+ PATH += "C:\Qt5\5.15.2\msvc2019_64\bin"
+ QTDir = "C:\Qt5\5.15.2\msvc2019_64"
+ QT_PLUGIN_PATH = "C:\Qt5\5.15.2\msvc2019_64\plugins\platforms"
+ QMAKESPEC = "win32-msvc"
+ QMAKE_MSC_VER = 1929

You can also set QMAKE_MSC_VER = 1929 in header of the file "C:\Qt5\5.15.2\msvc2019_64\mkspecs\common\msvc-version.conf"
