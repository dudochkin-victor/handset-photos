/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */


#ifndef ALBUMTHUMBNAILSCELLPROCESSOR_H
#define ALBUMTHUMBNAILSCELLPROCESSOR_H
#include <MContentItem>
#include "abstractcellprocessor.h"
#include "dynamicalbumlist.h"

class AlbumThumbnailsCellProcessor : public AbstractCellProcessor
{
public:
    AlbumThumbnailsCellProcessor(DynamicAlbumList *albumList = 0) : albumList(albumList){};
    virtual void processCell(MContentItem *cell);
private:
    DynamicAlbumList *albumList;
};

#endif // PHOTOTHUMBNAILSCELLPROCESSOR_H
