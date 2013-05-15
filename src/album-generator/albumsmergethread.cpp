/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */



#include <QDebug>
#include <QEventLoop>

#include "albumsmergethread.h"
#include "albumsmerger.h"
#include "../searchengine.h"
#include "../getallphotosworker.h"

void AlbumsMergeThread::run()
{
    QEventLoop loop;
    QVector<QStringList> *photos = NULL;
    GetAllPhotosWorker *worker = GetAllPhotosWorker::instance();
    if (worker != NULL) {
        worker->waitTillDone();
        photos = &(worker->allPhotos);
    }

    AlbumsMerger merger;
    if (photos) {
        QObject::connect(&merger, SIGNAL(albumsChanged()), this, SLOT(onAlbumsChanged()));
        QTime t;
        t.start();
        merger.merge(photos);
        qDebug() << "12345 merge elaped:" << t.elapsed();
    }
    loop.exec();
}
    
void AlbumsMergeThread::onAlbumsChanged()
{
    emit albumsChanged();
}
