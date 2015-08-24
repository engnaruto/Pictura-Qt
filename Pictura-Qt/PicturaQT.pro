#-------------------------------------------------
#
# Project created by QtCreator 2015-06-27T14:13:50
#
#-------------------------------------------------

QT       += core gui

QT += printsupport


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Pictura-Qt
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

CONFIG += mobility
MOBILITY =


RESOURCES += \
    pics.qrc

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

DISTFILES += \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/AndroidManifest.xml \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew \
    android/gradlew.bat
