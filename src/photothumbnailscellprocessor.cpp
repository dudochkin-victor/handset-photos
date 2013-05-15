/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */


#include <MAction>
#include "photothumbnailscellprocessor.h"
#include "appwindow.h"


void PhotoThumbnailsCellProcessor::processCell(MContentItem *cell)
{
    MAction *actionView = new MAction("Open", cell);
    actionView->setLocation(MAction::ObjectMenuLocation);
    QObject::connect(actionView, SIGNAL(triggered()),photoList, SLOT(objectMenuView()));
    cell->addAction(actionView);

    MAction *actionDelete = new MAction("Delete", cell);
    actionDelete->setLocation(MAction::ObjectMenuLocation);
    QObject::connect(actionDelete, SIGNAL(triggered()), photoList, SLOT(objectMenuDelete()));
    cell->addAction(actionDelete);
}
