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
#include "mcontentitemex.h"



DynamicPhotoList::DynamicPhotoList(MWidget *parent, M::Orientation orientation) : DynamicMList(parent)
{
    connect(this, SIGNAL(panningStopped()), this, SLOT(doTasklet()));
    //connect(AppWindow::instance(), SIGNAL(orientationChanged(M::Orientation)), this, SLOT(onOrientationChanged(M::Orientation)));
    photosListModel = new MPListModel(this);
    photosTasklet = new PhotosTasklet(this);
    setItemModel(photosListModel);
    setTasklet(photosTasklet);
    MContentItemCreator *photothumbnailCreator = dynamic_cast<MContentItemCreator *>(cellCreator());
    onOrientationChanged(orientation);
    setObjectName("thumbnailList");
    enableItemClick();
    QPixmapCache::setCacheLimit (4096);

    QTimer::singleShot(1500, this, SLOT(doTasklet()));
}

DynamicPhotoList::~DynamicPhotoList()
{
   //The base class's destructor takes care of the model and the tasklet
    disconnect(this, SIGNAL(panningStopped()), this, SLOT(doTasklet()));
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
}

void DynamicPhotoList::onClick(const QModelIndex &index)
{
    QVariant data = index.data(Qt::DisplayRole);
    MPListItem *item = static_cast<MPListItem *>(data.value<void *>());

    QString thumbnailPath = item->thumbnailURI;
    if (thumbnailPath.isEmpty())
        thumbnailPath = PhotosTasklet::getThumbnailPath(item->photoURI);
    emit clicked(item->photoURI, thumbnailPath);
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
    MPListModel *model = dynamic_cast<MPListModel *>(itemModel());

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
       if (!index.isValid())
           continue;
       QVariant data = index.data(Qt::DisplayRole);
       MPListItem *entry = static_cast<MPListItem *>(data.value<void *>());
       if (entry == NULL || entry->isLoaded) 
           continue; 
       model->setSpinner(index, true);
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
        model->setDefaultThumbnail(i);
    }

   
    // remove rows after lastVFlatRow + numItemsAfterLastvisibleOne
    for (int i = lastRow + 1; i < model->rowCount(); i++)
        model->setDefaultThumbnail(i);

    int i = lastRow + 1;
    if (i < model->rowCount()) {
       QModelIndex index(firstVisibleIndex.sibling(i, 0));
       if (index.isValid()) {
           QVariant data = index.data(Qt::DisplayRole);
           MPListItem *entry = static_cast<MPListItem *>(data.value<void *>());
           if (entry && !entry->isLoaded) 
            model->setSpinner(index, true);
       }
    }

    getTasklet()->processJobQueue();
}


void genCSSFile(const char *filename, int edge) {
const char *l1 = "@const THUMBNAIL_CELL_SIZE:";
const char *l2 = "#photoThumbnail2 { \n \
  minimum-size: $THUMBNAIL_CELL_SIZE $THUMBNAIL_CELL_SIZE; \n\
  preferred-size: $THUMBNAIL_CELL_SIZE $THUMBNAIL_CELL_SIZE; \n\
  maximum-size: $THUMBNAIL_CELL_SIZE $THUMBNAIL_CELL_SIZE; \n \
  image-object-name: \"PhotoThumbnailImage2\"; \n\
}\n \
#photoThumbnail2:pressed { \n\
  image-object-name: \"PhotoThumbnailImage2\"; \n\
} \n\
#PhotoThumbnailImage2 { \n\
  minimum-size: $THUMBNAIL_CELL_SIZE $THUMBNAIL_CELL_SIZE; \n\
  preferred-size: $THUMBNAIL_CELL_SIZE $THUMBNAIL_CELL_SIZE; \n\
  maximum-size: $THUMBNAIL_CELL_SIZE $THUMBNAIL_CELL_SIZE;\n\
}";

    QFile f(filename);
    if (f.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream out(&f);
        out << l1 << " " << edge << "px;" << "\n";
        out << l2 << "\n";
    }
    f.close();
}

