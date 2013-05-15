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
#include <MLabel>
#include "mcontentitemcreator.h"
#include "mcontentitemex.h"

void MContentItemCreator::updateCell(const QModelIndex &index, MWidget *cell) const
{
    MContentItemEx *contentItem = dynamic_cast<MContentItemEx *>(cell);
    if (contentItem == NULL) // TODO This is shouldn't happen, list must know what it doing, but with multiple columns it happens sometimes
        return;

    QVariant data = index.data(Qt::DisplayRole);
    MPListItem *item = static_cast<MPListItem *>(data.value<void *>());
    //contentItem->setItemMode(ContentItemMode);
    //qDebug() << "updateCell row:" << index.row() << "thumbnail" << item->thumbnailURI;
    //qDebug() << "cellSize" << cell->size().width() << "," << cell->size().height();
    contentItem->setPixmap(*(item->thumbnail));
    if (item->showSpinner) {
        contentItem->showSpinner();
    } else  {
        contentItem->hideSpinner();
    }

    //if (contentItem->itemStyle() ==  MContentItem::TwoIconsTwoWidgets) {
    if (id == 1) {
        QString text = "<b>" + item->albumName() + "</b>    ";
        contentItem->setTitle(text);
         //% "number of photos"
        text = " " + qtTrId("number of photos", item->numPhotos);
        contentItem->setSubtitle(text);
       /* int rowNum = index.row();
        rowNum /= 2;
        qDebug() << "rowNum is " << rowNum;
        if (rowNum & 1)
            contentItem->setObjectName("oddAlbumThumbnail"); */
        
        
    }
    // with implicit data sharing
    //all contentItem should share one copy of defaultThumbnail's data'
    contentItem->boundingRect();

    //updateContentItemMode(index, contentItem);
}

