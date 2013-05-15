include (../common.pri)
TARGET = allphotos
TEMPLATE = lib
target.path = /usr/lib

MOC_DIR = .moc
OBJECTS_DIR = .obj

QT += opengl network dbus

QMAKE_CXXFLAGS += -Wno-reorder

CONFIG += link_pkgconfig \
    qt \
    meegotouch \
    qdbus 

LIBS += -lqttracker

SOURCES += *.cpp

HEADERS += *.h

headers.files = $$HEADERS
headers.path = /usr/include

INSTALLS += target headers
