/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */


#ifndef PHOTOTHUMBNAILSCELLPROCESSOR_H
#define PHOTOTHUMBNAILSCELLPROCESSOR_H
#include <MContentItem>
#include "abstractcellprocessor.h"
#include "dynamicphotolist.h"

class PhotoThumbnailsCellProcessor : public AbstractCellProcessor
{
public:
    PhotoThumbnailsCellProcessor(DynamicPhotoList *photoList = 0) : photoList(photoList){};
    virtual void processCell(MContentItem *cell);
private:
    DynamicPhotoList *photoList;
};

#endif // PHOTOTHUMBNAILSCELLPROCESSOR_H
