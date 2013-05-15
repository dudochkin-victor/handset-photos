include (../common.pri)
TARGET = meegophotos
TEMPLATE = app
target.path = /usr/bin

MOC_DIR = .moc
OBJECTS_DIR = .obj

theme.files = themes/*
theme.path = /usr/share/themes/base/meegotouch/$$TARGET

desktop.files = data/*.desktop
desktop.path = /usr/share/applications

# Icon
desktop_icon.files = ./themes/images/icons-Applications-photos.png
desktop_icon.path = /usr/share/pixmaps
desktop_icon.CONFIG += no_check_exist


QT += opengl network dbus

QMAKE_CXXFLAGS += -Wno-reorder

CONFIG += link_pkgconfig \
    qt \
    meegotouch \
    qdbus 

PKGCONFIG += libexif

LIBS += -lqttracker

DBUS_ADAPTORS += dbus/com.meego.photos.xml

system(qdbusxml2cpp -a photos_adaptor.h: dbus/com.meego.photos.xml)
system(qdbusxml2cpp -p flickr_interface.h:flickr_interface.cpp interfaces/org.moblin.libsocialweb.service.flickr.xml -i dbustypes.h)

SOURCES += main.cpp \
  appwindow.cpp \
  photostrip.cpp \
  photostripview.cpp \
  photostripmodel.cpp \
  dynamicmlist.cpp \
  photostasklet.cpp \
  abstracttasklet.cpp \
  mplistmodel.cpp \
  mcontentitemcreator.cpp \
  dynamicphotolist.cpp \
  dynamicalbumlist.cpp \
  photothumbnailscellprocessor.cpp \
  albumthumbnailscellprocessor.cpp \
  stripitem.cpp \
  searchengine.cpp \
  thumbnailer.cpp \
  photostripphysics.cpp \
  photostriplayout.cpp \
  mplabelwithclick.cpp \
  "album-generator/albumsmerger.cpp" \
  "album-generator/albumsmergethread.cpp" \
  stripimageview.cpp \
  mpsettings.cpp \
  mcontentitemex.cpp \
  trackerlistener.cpp \
  photolistdeleteconfirm.cpp \
  searchresults.cpp \
  mpproxylistmodel.cpp \
  exifutils.cpp \
  getallphotosworker.cpp \
  "album-generator/yahooreversegeocoder.cpp" \
  sharingactionfactory.cpp \
  socialuploaderpage.cpp \
  albumlistdeleteconfirm.cpp \
  flickr_interface.cpp \
  socialwebservice.cpp \
  $$DBUS_ADAPTOR_SOURCES \
  mpsinglephotomodel.cpp \
  photostrippage.cpp


HEADERS += \
  appwindow.h \
  photostrip.h \
  photostripview.h \
  photostripmodel.h \
  photostripstyle.h \
  dynamicmlist.h \
  abstracttasklet.h \
  photostasklet.h \
  mcontentitemcreator.h \
  mplistmodel.h \
  mcontentitemcreator.h \
  dynamicphotolist.h \
  dynamicalbumlist.h \
  searchengine.h \
  photothumbnailscellprocessor.h \
  albumthumbnailscellprocessor.h \
  stripitem.h \
  thumbnailer.h \
  photostripphysics.h \
  photostriplayout.h \
  mplabelwithclick.h \
  "album-generator/albumsmerger.h" \
  "album-generator/albumsmergethread.h" \
  stripimageview.h \
  stripimagestyle.h \
  mpsettings.h \
  mcontentitemex.h \
  trackerlistener.h \
  abstractdeleteconfirm.h \
  photolistdeleteconfirm.h \
  searchresults.h \
  mpproxylistmodel.h \
  mpabstractlistmodelops.h \
  exifutils.h \
  getallphotosworker.h \
  "album-generator/yahooreversegeocoder.h" \
  "album-generator/reversegeocoder.h" \
  sharingactionfactory.h \
  socialuploaderpage.h \
  albumlistdeleteconfirm.h \
  dbustypes.h \
  flickr_interface.h \
  socialwebservice.h \
  $$DBUS_INTERFACE_HEADERS \
  $$DBUS_ADAPTOR_HEADERS \
  mpsinglephotomodel.h \
  photostrippage.h


MODEL_HEADERS += \
  photostripmodel.h

STYLE_HEADERS += \
  photostripstyle.h \
  stripimagestyle.h

INSTALLS += target theme desktop desktop_icon

MGEN_OUTDIR = .gen
mgen.output = $$GEN_DIR/gen_${QMAKE_FILE_BASE}data.cpp
mgen.clean += $$GEN_DIR/gen_*

