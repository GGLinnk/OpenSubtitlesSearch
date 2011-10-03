# -------------------------------------------------
# Project created by QtCreator 2010-12-31T19:16:57
# -------------------------------------------------
# QT += xml network
include (src/client/client.pri)
DEFINES += USE_ICU
contains(DEFINES, USE_ICU):include(src/icu/icu.pri)
TARGET = OpenSubtitlesSearch
TEMPLATE = app
INCLUDEPATH += src
SOURCES += src/main.cpp \
    src/mainwindow.cpp \
    src/subtitleslistmodel.cpp \
    src/columnssubtitleslistmodel.cpp \
    src/propertiesdialog.cpp \
    src/columnssubtitlesview.cpp \
    src/subtitlesview.cpp \
    src/lineeditfilechoser.cpp \
    src/networkreplydownloadtablemodel.cpp \
    src/networkreplydownloaddelegate.cpp \
    src/networkreplydownloadview.cpp \
    src/settings.cpp \
    src/gestionconnexion.cpp \
    src/subtitledetail.cpp \
    src/aproposdialog.cpp \
    src/updatemoviehashdialog.cpp \
    src/updatemoviehashform.cpp \
    src/searchdialog.cpp \
    src/addcommentdialog.cpp \
    src/generated/oss_langs.cpp
HEADERS += src/mainwindow.h \
    src/subtitleslistmodel.h \
    src/columnssubtitleslistmodel.h \
    src/propertiesdialog.h \
    src/columnssubtitlesview.h \
    src/subtitlesview.h \
    src/lineeditfilechoser.h \
    src/networkreplydownloadtablemodel.h \
    src/networkreplydownloaddelegate.h \
    src/networkreplydownloadview.h \
    src/settings.h \
    src/gestionconnexion.h \
    src/subtitledetail.h \
    src/aproposdialog.h \
    src/updatemoviehashdialog.h \
    src/updatemoviehashform.h \
    src/searchdialog.h \
    src/addcommentdialog.h \
    src/generated/oss_langs.h
FORMS += ui/mainwindow.ui \
    ui/propertiesdialog.ui \
    ui/aproposdialog.ui \
    ui/updatemoviehashdialog.ui \
    ui/updatemoviehashform.ui \
    ui/searchdialog.ui \
    ui/searchByHash.ui \
    ui/searchByQuery.ui \
    ui/searchbyMixed.ui \
    ui/addcommentdialog.ui
RESOURCES += resources.qrc
OBJECTS_DIR = tmp
MOC_DIR = tmp
UI_DIR = tmp
RCC_DIR = tmp
TRANSLATIONS = translations/OpenSubtitlesSearch_fr.ts
