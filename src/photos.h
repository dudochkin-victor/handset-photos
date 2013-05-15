/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */


#ifndef PHOTOS_H
#define PHOTOS_H

#include <MWidgetController>
#include "photosview.h"
#include "photosmodel.h"

class M_EXPORT Photos : public MWidgetController
{
    Q_OBJECT
    M_CONTROLLER(Photos);

public:
    Photos(QGraphicsItem *parent = 0);
    virtual ~Photos();
    const PhotosView *getView() {return dynamic_cast<const PhotosView *>(view()); };

public Q_SLOTS:
    void setQuery(QString query) {
	    model()->setQuery(query);
    };
    void showPhoto();
    void showPhotoFromObjectMenu();
    void deletePhoto();

private:
    friend class PhotosView;
};

#endif // PHOTOS_H
