/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */


#ifndef ALBUMS_H
#define ALBUMS_H

#include <MWidgetController>
#include "albumsview.h"
#include "albumsmodel.h"

class M_EXPORT Albums : public MWidgetController
{
    Q_OBJECT
    M_CONTROLLER(Albums);

public:
    Albums(QGraphicsItem *parent = 0);
    virtual ~Albums();

public Q_SLOTS:
    void setQuery(QString query) {
	    model()->setQuery(query);
    };
/*    void showAlbum(); */
    void showAlbumFromObjectMenu();
    void renameAlbum();
    void deleteAlbum();

private:
    friend class AlbumsView;
};

#endif // ALBUMS_H
