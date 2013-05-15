/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef PHOTOSTASKLET_H
#define PHOTOSTASKLET_H

#include "abstracttasklet.h"
#include "dynamicphotolist.h"

class DynamicPhotoList;
struct ThumbnailCheckJob : public Job
{
    ThumbnailCheckJob(Job &j, int dueSec = 3) : Job(j) { dueTime = QTime::currentTime().addSecs(dueSec);};

    QTime dueTime;  //check the thumbnail file before this time
};

class PhotosTasklet : public AbstractTasklet
{
public:
    explicit PhotosTasklet(DynamicPhotoList *parent = 0);
    virtual ~PhotosTasklet();

    virtual void processSingleJob(Job& j);
    static const int CHECK_THUMBNAIL_FILE_INTERVAL = 100;
    static QPixmap squareQPixmapFromPath(QString path, qreal edgeR);
    static QString getThumbnailPath(const QString &uri);

private:
    DynamicPhotoList *list;
    QList<ThumbnailCheckJob> thumbnailsToBeGenerated;
    bool outstandingChecking;
};

#endif // PHOTOSTASKLET_H
