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

#include "photos.h"
#include "appwindow.h"

Photos::Photos(QGraphicsItem *parent) :
  MWidgetController(new PhotosModel(), parent)
{
}

Photos::~Photos()
{
}

/*
void Photos::showPhoto()
{
    QString res = sender()->objectName();

    AppWindow *window = AppWindow::instance();
    window->showSinglePhoto(res);
}
*/

void Photos::showPhotoFromObjectMenu()
{
    // Since we are dealing with a slot triggered by 
    // an objectmenu action, then find the res string
    // by looking in the parent of the sender
    QString res = sender()->parent()->objectName();

    AppWindow *window = AppWindow::instance();
    window->showSinglePhoto(res);    
}

void Photos::deletePhoto()
{
    QString res = sender()->parent()->objectName();

    qDebug() << "deletePhoto : " << res;

    // open a confirmation dialog and then delete the photo
    // if confirmed
}
