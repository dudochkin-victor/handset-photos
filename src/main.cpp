/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */


#include <mapplication.h>
#include <MApplicationWindow>
#include <QDesktopServices>
#include <QUrl>
#include <QObject>
#include "album-generator/albumsmergethread.h"

#include "appwindow.h"
#include "getallphotosworker.h"
#include "dbustypes.h"
#include "flickr_interface.h"
#include "socialwebservice.h"
#include <QtDBus>

int main(int argc, char *argv[])
{
    MApplication app(argc, argv);
    bool isSinglePhotoMode = false;
    for (int i = 0; i < argc; i++)
        if (QString(argv[i]) == "-single-photo") {
            isSinglePhotoMode = true; 
        }

    QCoreApplication::setOrganizationName("Intel");
    QCoreApplication::setOrganizationDomain("www.intel.com");
    QCoreApplication::setApplicationName("Meego Photos");

    GetAllPhotosWorker::instance()->start(QThread::HighPriority);

    qDBusRegisterMetaType<FlickrStruct>();
    AppWindow w(isSinglePhotoMode);
    w.show();

    QDBusConnection::sessionBus().registerService("com.meego.photos");
    QDBusConnection::sessionBus().registerObject("/", &w);


    if (!isSinglePhotoMode) {
        AlbumsMergeThread *albumsMergeThread;
        albumsMergeThread = new AlbumsMergeThread();
        MPListModel *albumModel = dynamic_cast<MPListModel *>(w.albumList->itemModel());
        QObject::connect(albumsMergeThread, SIGNAL(albumsChanged()), albumModel, SLOT(updateAlbumModel()), Qt::QueuedConnection);
        albumsMergeThread->start();
    }
    return app.exec();
}
