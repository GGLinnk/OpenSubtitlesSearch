
INCLUDEPATH += $$PWD

SOURCES += $$PWD/icu_charset.cpp
HEADERS += $$PWD/icu_charset.h

win32 {
    INCLUDEPATH += icu46_win32/include
    LIBS += -L$$PWD/../../icu46_win32/lib -licuuc -licudt -licuin
} else {
    LIBS += -licuuc -licui18n
}

