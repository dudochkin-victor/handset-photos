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
#include <QTimer>
#include <QPixmapCache>
#include <MWidgetView>
#include "dynamicphotolist.h"
#include "dynamicmlist.h"
#include "mcontentitemcreator.h"
#include "appwindow.h"
#include "photothumbnailscellprocessor.h"
#include "mcontentitemex.h"
#include "photolistdeleteconfirm.h"



DynamicPhotoList::DynamicPhotoList(MWidget *parent, QString albumID) : DynamicMList(parent), photoListDeleteConfirm(NULL), firstRow(-1), lastRow(-2)
{
    connect(this, SIGNAL(panningStopped()), this, SLOT(doTasklet()));
    connect(AppWindow::instance(), SIGNAL(orientationChanged(M::Orientation)), this, SLOT(onOrientationChanged(M::Orientation)));
    photosListModel = new MPListModel(this, albumID);
    photosTasklet = new PhotosTasklet(parent);
    setItemModel(photosListModel);
    setTasklet(photosTasklet);
    MContentItemCreator *photothumbnailCreator = dynamic_cast<MContentItemCreator *>(cellCreator());
    photothumbnailCreator->setCellProcessor(new PhotoThumbnailsCellProcessor(this));
    onOrientationChanged(AppWindow::instance()->orientation());
    setObjectName("thumbnailList");
    enableItemClick();
    QPixmapCache::setCacheLimit (4096);

    QTimer::singleShot(1500, this, SLOT(doTasklet()));
}

DynamicPhotoList::~DynamicPhotoList()
{
   //The base class's destructor takes care of the model and the tasklet
    disconnect(this, SIGNAL(panningStopped()), this, SLOT(doTasklet()));
    if (photoListDeleteConfirm)
        delete photoListDeleteConfirm;
}

void DynamicPhotoList::enableItemClick()
{
    connect(this, SIGNAL(itemClicked(const QModelIndex&)), this, SLOT(onClick(const QModelIndex&)));
}

void DynamicPhotoList::disableItemClick()
{
    disconnect(this, SIGNAL(itemClicked(const QModelIndex&)), this, SLOT(onClick(const QModelIndex&)));
}

void DynamicPhotoList::onOrientationChanged(M::Orientation orientation)
{
    MContentItemCreator *photothumbnailCreator = dynamic_cast<MContentItemCreator *>(cellCreator());
    if (orientation == M::Portrait)
        photothumbnailCreator->setCellObjectName("photoThumbnail_Portrait");
    else
        photothumbnailCreator->setCellObjectName("photoThumbnail_Landscape");

    setColumns(orientation == M::Portrait ? 5 : 8);
    MPAbstractListModelOps *modelOps = dynamic_cast<MPAbstractListModelOps *>(itemModel());
    QAbstractItemModel *model = dynamic_cast<QAbstractItemModel *>(itemModel());
    if (modelOps && model) {
        for (int i = firstRow; i <= lastRow; i++)
            modelOps->setDefaultThumbnail(i);

        QTimer::singleShot(0, this, SLOT(doTasklet()));
    }
}

void DynamicPhotoList::onClick(const QModelIndex &index)
{
    AppWindow *window = AppWindow::instance();
    window->showSinglePhoto(const_cast<const QAbstractItemModel *>(itemModel()), index);
}

void DynamicPhotoList::objectMenuView()
{
    MContentItemEx *itemEx = dynamic_cast<MContentItemEx *>(sender()->parent());
    AppWindow::instance()->showSinglePhoto(const_cast<const QAbstractItemModel *>(itemModel()), itemEx->index);
}

void DynamicPhotoList::objectMenuDelete()
{
    MContentItemEx *itemEx = dynamic_cast<MContentItemEx *>(sender()->parent());
    QVariant data = itemEx->index.data(Qt::DisplayRole);
    MPListItem *entry = static_cast<MPListItem *>(data.value<void *>());
    if (entry) {
        QString fileString = entry->photoURI.remove(QRegExp("^file:\/\/"));
        if (!photoListDeleteConfirm)
            photoListDeleteConfirm = new PhotoListDeleteConfirm();
        photoListDeleteConfirm->setFile(fileString);
        //AppWindow::instance()->deletePhoto("Delete1 Photo", qtTrId("Are you sure to delete") + " " + fileString + "?", photoListDeleteConfirm);
        
        //% "Are you sure you want to delete this photo"
        AppWindow::instance()->deletePhoto("", qtTrId("Are you sure you want to delete this photo") + "?", photoListDeleteConfirm);
    }
}

const int DynamicPhotoList::numItemsBeforeFirstVisibleOne = 4*8; 
const int DynamicPhotoList::numItemsAfterLastVisibleOne = 4*8;

#define Q_JOB()    do { \
    QModelIndex index(firstVisibleIndex.sibling(i, 0)); \
        if (!index.isValid())\
            continue;\
        QVariant data = index.data(Qt::DisplayRole);\
        MPListItem *entry = static_cast<MPListItem *>(data.value<void *>());\
        if (entry == NULL || entry->isLoaded) \
            continue; \
 \
\
        Job job; \
        job.userData = data; \
        job.row = index; \
        getTasklet()->addJob(job); \
}  while (0) 

void DynamicPhotoList::doTasklet()
{
    QModelIndex firstVisibleIndex = firstVisibleItem();
    QModelIndex lastVisibleIndex = lastVisibleItem();

    int firstVFlatRow = firstVisibleIndex.row();
    int lastVFlatRow = lastVisibleIndex.row();
    if (lastVFlatRow - firstVFlatRow < 23) {
        //when the doTasklet gets called in the timeout handler,
        //the lastVisbieItem() has not been calculated yet by MListFastView
        lastVFlatRow = firstVFlatRow + 23;  //FixMe: Arbitrary number.
    }
    qDebug() << "~~~~~~do Tasklet" ;
    QAbstractItemModel *model = dynamic_cast<QAbstractItemModel *>(itemModel());
    MPAbstractListModelOps *modelOps = dynamic_cast<MPAbstractListModelOps *>(itemModel());

    // remove rows before firstVFlatRow - numItemsBeforeFirstVisibleOne
    firstRow = firstVFlatRow - numItemsBeforeFirstVisibleOne;
    if (firstRow < 0)
        firstRow = 0;

    lastRow = lastVFlatRow + numItemsAfterLastVisibleOne;
    if (lastRow >= model->rowCount())
        lastRow = model->rowCount() - 1;
 

   //Q tasklets
   for (int i = firstVFlatRow; i <= lastRow; i++) {
       QModelIndex index(firstVisibleIndex.sibling(i, 0));
       if (!index.isValid())
           continue;
       QVariant data = index.data(Qt::DisplayRole);
       MPListItem *entry = static_cast<MPListItem *>(data.value<void *>());
       if (entry == NULL || entry->isLoaded) 
           continue; 
       modelOps->setSpinner(index, true);
       break;
   }
    for (int i = firstVFlatRow; i <= lastVFlatRow; i++)
        Q_JOB();
    for (int i = lastVFlatRow + 1; i  <= lastRow; i++)
        Q_JOB();
    for (int i = firstRow; i < firstVFlatRow; i++)
        Q_JOB();

    for (int i = 0; i < firstRow; i++) {
        if (i >= model->rowCount())
            break;

        //I guess when assign a new content to pixmap, it should release the orignal buffer
        // and share the new content
        modelOps->setDefaultThumbnail(i);
    }

   
    // remove rows after lastVFlatRow + numItemsAfterLastvisibleOne
    for (int i = lastRow + 1; i < model->rowCount(); i++)
        modelOps->setDefaultThumbnail(i);

    int i = lastRow + 1;
    if (i < model->rowCount()) {
       QModelIndex index(firstVisibleIndex.sibling(i, 0));
       if (index.isValid()) {
           QVariant data = index.data(Qt::DisplayRole);
           MPListItem *entry = static_cast<MPListItem *>(data.value<void *>());
           if (entry && !entry->isLoaded) 
            modelOps->setSpinner(index, true);
       }
    }

    getTasklet()->processJobQueue();
}

void DynamicPhotoList::setAlbumUrn(QString newID)
{
    MPListModel *model;
    model = dynamic_cast<MPListModel *>(itemModel());
    photosListModel = new MPListModel(this, newID);
    setItemModel(photosListModel);
    delete model;
    photosTasklet->stopJobQueue();
    QTimer::singleShot(0, this, SLOT(doTasklet()));
}
