/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */


#ifndef DYNAMICALBUMLIST_H
#define DYNAMICALBUMLIST_H

#include <QObject>
#include "dynamicmlist.h"
#include "mplistmodel.h"
#include "photostasklet.h"
#include "albumlistdeleteconfirm.h"

class DynamicAlbumList : public DynamicMList
{
    Q_OBJECT
public:
    DynamicAlbumList(MWidget *parent = 0);

    virtual ~DynamicAlbumList();

    static const int numItemsBeforeFirstVisibleOne;
    static const int numItemsAfterLastVisibleOne;
    void getPhotosToBeShared(QStringList &photos);
    void getPhotoThumbnailToBeShared(QStringList &photos);

public slots:
    virtual void doTasklet();
    void onClick(const QModelIndex &index);
    void objectMenuOpen();
    void objectMenuShare();
    void objectMenuDelete();
//    void updateAlbumModel();
    void onOrientationChanged(M::Orientation orientation);


private:
//    MPListModel *albumListModel;
    PhotosTasklet *albumTasklet;   //will change the name later
    QStringList photosToBeShared;
    AlbumListDeleteConfirm *albumListDeleteConfirm;
};

#endif 
