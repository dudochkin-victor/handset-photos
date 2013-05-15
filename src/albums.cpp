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

#include "albums.h"
#include "appwindow.h"

Albums::Albums(QGraphicsItem *parent) :
  MWidgetController(new AlbumsModel(), parent)
{
}

Albums::~Albums()
{
}

/* void Albums::showAlbum()
{
    QString res = sender()->objectName();

    AppWindow *window = AppWindow::instance();
    window->showAlbum(res);
}
*/

#if 0
void Albums::showAlbumFromObjectMenu()
{
    // Since we are dealing with a slot triggered by 
    // an objectmenu action, then find the res string
    // by looking in the parent of the sender
    QString res = sender()->parent()->objectName();

    AppWindow *window = AppWindow::instance();
    window->showAlbum(res);    
}
#endif

void Albums::renameAlbum()
{
    QString res = sender()->parent()->objectName();

    qDebug() << "renameAlbum : " << res;

    // open a dialog asking for the new name
}

void Albums::deleteAlbum()
{
    QString res = sender()->parent()->objectName();

    qDebug() << "deleteAlbum : " << res;

    // open a confirmation dialog and then delete the album
    // if confirmed
}
