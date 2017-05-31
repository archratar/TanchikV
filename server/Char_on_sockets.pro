TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CFLAGS += -std=c11

SOURCES += \
    main.c \
    server_tools.c \
    debug.c

    
# must use the path to your "ws2_32,dll"
win32: LIBS += -L$$PWD/C:/Programs/IDE/Qt/Tools/mingw492_32/i686-w64-mingw32/lib/ -lwsock32

INCLUDEPATH += $$PWD/C:/Programs/IDE/Qt/Tools/mingw492_32/i686-w64-mingw32/include
DEPENDPATH += $$PWD/C:/Programs/IDE/Qt/Tools/mingw492_32/i686-w64-mingw32/include

HEADERS += \
    server_tools.h \
    debug.h
