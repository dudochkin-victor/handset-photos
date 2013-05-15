/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */


#ifndef DYNAMICPHOTOLIST_H
#define DYNAMICPHOTOLIST_H

#include <QObject>
#include "dynamicmlist.h"
#include "mplistmodel.h"
#include "photostasklet.h"

class PhotosTasklet;
class DynamicPhotoList : public DynamicMList
{
    Q_OBJECT
public:
    DynamicPhotoList(MWidget *parent = 0, M::Orientation orientation = M::Landscape);

    virtual ~DynamicPhotoList();

    static const int numItemsBeforeFirstVisibleOne;
    static const int numItemsAfterLastVisibleOne;

    void setAlbumUrn(QString albumUrn);
    void enableItemClick();
    void disableItemClick();

public slots:
    virtual void doTasklet();
    void onClick(const QModelIndex &index);
    void onOrientationChanged(M::Orientation orientation);

Q_SIGNALS:
    void clicked(const QString &photoURI, const QString &thumbnailPath);

private:
    MPListModel *photosListModel;
    PhotosTasklet *photosTasklet;
};

#endif // DYNAMICPHOTOLIST_H
