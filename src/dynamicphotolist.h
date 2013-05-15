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
#include "photolistdeleteconfirm.h"

class DynamicPhotoList : public DynamicMList
{
    Q_OBJECT
public:
    DynamicPhotoList(MWidget *parent = 0, QString albumID="");

    virtual ~DynamicPhotoList();

    static const int numItemsBeforeFirstVisibleOne;
    static const int numItemsAfterLastVisibleOne;

    void setAlbumUrn(QString albumUrn);
    void enableItemClick();
    void disableItemClick();

public slots:
    virtual void doTasklet();
    void onClick(const QModelIndex &index);
    void objectMenuView();
    void objectMenuDelete();
    void onOrientationChanged(M::Orientation orientation);


private:
    MPListModel *photosListModel;
    PhotosTasklet *photosTasklet;
    PhotoListDeleteConfirm *photoListDeleteConfirm;

    int firstRow;
    int lastRow;
};

#endif // DYNAMICPHOTOLIST_H
