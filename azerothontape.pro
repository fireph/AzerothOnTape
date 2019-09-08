HEADERS   = AzerothOnTapeSettingsDialog.hpp \
            FileDownloader.hpp
SOURCES   = main.cpp \
            AzerothOnTapeSettingsDialog.cpp \
            FileDownloader.cpp
RESOURCES = azerothontape.qrc

CONFIG(debug, debug|release) {
    DESTDIR = debug/
} else {
    DESTDIR = release/
}

OBJECTS_DIR = $$DESTDIR/.obj
MOC_DIR = $$DESTDIR/.moc
RCC_DIR = $$DESTDIR/.qrc
UI_DIR = $$DESTDIR/.ui

VERSION = 1.0.0
QMAKE_TARGET_COMPANY = "Frederick Meyer"
QMAKE_TARGET_PRODUCT = "Azeroth On Tape"
QMAKE_TARGET_DESCRIPTION = "Azeroth On Tape"
QMAKE_TARGET_COPYRIGHT = "\251 2019 Frederick Meyer"

#CONFIG += console

CONFIG += openssl-linked

INCLUDEPATH += C:/OpenSSL-Win64/include

msvc: LIBS += -LC:/OpenSSL-Win64/lib -luser32 -lVersion

win32: RC_ICONS += images/quest.ico

QT += core widgets network multimedia
requires(qtConfig(combobox))

include(vendor/vendor.pri)
