QT += xml network

win32 {
    INCLUDEPATH += zlib-win32/include
} else {
    LIBS += -lz
}

HEADERS += $$PWD/OpenSubtitlesRPCClient.h $$PWD/OpenSubtitlesHelper.h
SOURCES += $$PWD/OpenSubtitlesRPCClient.cpp $$PWD/XmlHelper.cpp $$PWD/RCPClient.cpp  $$PWD/OpenSubtitlesHelper.cpp
