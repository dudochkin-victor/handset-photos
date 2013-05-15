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
#include "albumthumbnailscellprocessor.h"
#include "appwindow.h"


void AlbumThumbnailsCellProcessor::processCell(MContentItem *cell)
{
    //% "Open"
    MAction *actionOpen = new MAction(qtTrId("Open"), cell);
    actionOpen->setLocation(MAction::ObjectMenuLocation);
    QObject::connect(actionOpen, SIGNAL(triggered()), albumList, SLOT(objectMenuOpen()));
    cell->addAction(actionOpen);
    
    //% "Share"
    MAction *actionShare = new MAction(qtTrId("Share"), cell);
    actionShare->setLocation(MAction::ObjectMenuLocation);
    QObject::connect(actionShare, SIGNAL(triggered()), albumList, SLOT(objectMenuShare()));
    cell->addAction(actionShare);

    //% "Delete"
    MAction *actionDelete = new MAction(qtTrId("Delete"), cell);
    actionDelete->setLocation(MAction::ObjectMenuLocation);
    QObject::connect(actionDelete, SIGNAL(triggered()), albumList, SLOT(objectMenuDelete()));
    cell->addAction(actionDelete);
}
