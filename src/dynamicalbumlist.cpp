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
#include "dynamicalbumlist.h"
#include "dynamicmlist.h"
#include "mcontentitemcreator.h"
#include "appwindow.h"
#include "albumthumbnailscellprocessor.h"
#include "albumlistdeleteconfirm.h"
#include "mcontentitemex.h"



DynamicAlbumList::DynamicAlbumList(MWidget *parent) : DynamicMList(parent,  MContentItem::IconAndTwoTextLabels, 1), albumListDeleteConfirm(NULL)
{
    connect(this, SIGNAL(panningStopped()), this, SLOT(doTasklet()));
    connect(AppWindow::instance(), SIGNAL(orientationChanged(M::Orientation)), this, SLOT(onOrientationChanged(M::Orientation)));
    MPListModel *albumListModel = new MPListModel(this, "", MPListModel::Album);
    albumTasklet = new PhotosTasklet(parent);
    setItemModel(albumListModel);
    setTasklet(albumTasklet);
    MContentItemCreator *albumThumbnailCreator = dynamic_cast<MContentItemCreator *>(cellCreator());
    albumThumbnailCreator->setCellProcessor(new AlbumThumbnailsCellProcessor(this));
    onOrientationChanged(AppWindow::instance()->orientation());

    setColumns(2);
    setObjectName("albumList");
    connect(this, SIGNAL(itemClicked(const QModelIndex&)), this, SLOT(onClick(const QModelIndex&)));
    QPixmapCache::setCacheLimit (4096);

    QTimer::singleShot(1500, this, SLOT(doTasklet()));
}

DynamicAlbumList::~DynamicAlbumList()
{
   //The base class's destructor takes care of the model and the tasklet
    disconnect(this, SIGNAL(panningStopped()), this, SLOT(doTasklet()));
}

void DynamicAlbumList::onOrientationChanged(M::Orientation orientation)
{
    MContentItemCreator *albumThumbnailCreator = dynamic_cast<MContentItemCreator *>(cellCreator());
    if (orientation == M::Portrait)
        albumThumbnailCreator->setCellObjectName("albumThumbnail_Portrait");
    else
        albumThumbnailCreator->setCellObjectName("albumThumbnail_Landscape");

    MPAbstractListModelOps *modelOps = dynamic_cast< MPAbstractListModelOps *>(itemModel());
    modelOps->relayout();
    //updateAlbumModel(); //force MList to relayout
}

void DynamicAlbumList::onClick(const QModelIndex &index)
{
    AppWindow *window = AppWindow::instance();
    MPListItem *item = static_cast<MPListItem *>(itemModel()->data(index, Qt::DisplayRole).value<void *>());
    window->showAlbum(item->urn);
}

void DynamicAlbumList::objectMenuOpen()
{
    MContentItemEx *itemEx = dynamic_cast<MContentItemEx *>(sender()->parent());
    if (itemEx) {
        onClick(itemEx->index);
    }
}

void DynamicAlbumList::objectMenuShare()
{
    MContentItemEx *itemEx = dynamic_cast<MContentItemEx *>(sender()->parent());
    QVariant data = (itemEx->index).data(Qt::DisplayRole);
    MPListItem *item = static_cast<MPListItem *>(data.value<void *>());
    QString albumID = AppWindow::instance()->settings.getAlbumID(item->urn);
    
    QVector<QStringList> result;
    SearchEngine::getAlbumPhotos(albumID, result);
    photosToBeShared.clear();
    for (int i = 0; i < result.count(); i++) {
        QString photoURI = result[i][0];
        photosToBeShared << photoURI;
    }
    AppWindow::instance()->share();
}


void DynamicAlbumList::objectMenuDelete()
{
    MContentItemEx *itemEx = dynamic_cast<MContentItemEx *>(sender()->parent());
    QVariant data = (itemEx->index).data(Qt::DisplayRole);
    MPListItem *item = static_cast<MPListItem *>(data.value<void *>());
    if (!albumListDeleteConfirm)
        albumListDeleteConfirm = new AlbumListDeleteConfirm();
    albumListDeleteConfirm->setAlbum(item->urn, itemEx->index);
    //AppWindow::instance()->deletePhoto("Delete Album", qtTrId("Are you sure to delete") + " " + item->albumName() + "?", albumListDeleteConfirm);
    //% "Are you sure you want to delete this album"
    AppWindow::instance()->deletePhoto("", qtTrId("Are you sure you want to delete this album") + "?", albumListDeleteConfirm);
}

const int DynamicAlbumList::numItemsBeforeFirstVisibleOne = 4*2; 
const int DynamicAlbumList::numItemsAfterLastVisibleOne = 4*2;

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
        qDebug() << "Q_JOB for album entry" << entry; \
        getTasklet()->addJob(job); \
}  while (0) 

void DynamicAlbumList::doTasklet()
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
    QAbstractItemModel *model = itemModel();
    MPAbstractListModelOps *modelOps = dynamic_cast<MPAbstractListModelOps *>(model);

    // remove rows before firstVFlatRow - numItemsBeforeFirstVisibleOne
    int firstRow = firstVFlatRow - numItemsBeforeFirstVisibleOne;
    if (firstRow < 0)
        firstRow = 0;

    int lastRow = lastVFlatRow + numItemsAfterLastVisibleOne;
    if (lastRow >= model->rowCount())
        lastRow = model->rowCount() - 1;
 

   //Q tasklets
    for (int i = firstVFlatRow; i <= lastRow; i++) {
       QModelIndex index(firstVisibleIndex.sibling(i, 0));
       if (!index.isValid()) {
           continue;
       }
       QVariant data = index.data(Qt::DisplayRole);
       MPListItem *entry = static_cast<MPListItem *>(data.value<void *>());
       if (entry)
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
            if (entry && !entry->isLoaded) {
                modelOps->setSpinner(index, true);
            }
        }
    }

    getTasklet()->processJobQueue();
}

/*
void DynamicAlbumList::updateAlbumModel()
{
    qDebug() << "DynamicAlbumList::updateAlbumModel";
    getTasklet()->stopJobQueue();   //The jobq may refer the old model
    MPListModel *old = dynamic_cast<MPListModel *>(itemModel());
    MPListModel *albumListModel = new MPListModel(this, "", MPListModel::Album);
    setItemModel(albumListModel);
    qDebug() << "DynamicAlbumList::updateAlbumModel set model" << albumListModel;
    QTimer::singleShot(400, this, SLOT(doTasklet()));
    if (old)
        delete old;
}
*/


void DynamicAlbumList::getPhotosToBeShared(QStringList &photos)
{
    photos = photosToBeShared;
}

void DynamicAlbumList::getPhotoThumbnailToBeShared(QStringList &photos)
{
    photos.clear();
    for (int i = 0; i < photosToBeShared.count(); i++) {
        photos << PhotosTasklet::getThumbnailPath(photosToBeShared[i]);
    }
}

