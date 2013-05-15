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
#include <QGraphicsGridLayout>

#include <MAction>
#include <MApplicationPage>
#include <MContentItem>
#include <MWidgetController>
#include <MTheme>

#include "albums.h"
#include "albumsview.h"

/*************************************************************************
 * AlbumsView
 *************************************************************************/
AlbumsView::AlbumsView(Albums *controller) :
  MWidgetView(controller),
  controller(controller)
{
    /*
     * Fill in some fake data to enable progress in developing the main UI
     */

    layout = new QGraphicsGridLayout(controller);
    layout->setSpacing(10);
}

AlbumsView::~AlbumsView()
{
}

void AlbumsView::updateData(const QList<const char*>& modifications)
{
    /*
    qDebug() << "AlbumView::updateData: " << modifications.count();

    MWidgetView::updateData(modifications);

    const char *member;
    foreach (member, modifications) {
	qDebug() << "updateData entry : " << member;
	if (member == AlbumsModel::Query)
	{
	    updateView(model()->query());
	}

    }
    */
}

void AlbumsView::updateView(QString query)
{
    /*

    qDebug() << "AlbumsView::updateView : " << query;

    while(layout->count())
	layout->removeAt(0);

    // Fill in some fake albums
    for (int j=0; j<5; j++)
    {
	for (int k=0; k<3; k++)
	{
	    MContentItem *item = new MContentItem;
	    item->setPixmap(*MTheme::pixmap("album"));
	    item->setTitle(QString("Title: %1").arg(j + k + 1));
	    item->setSubtitle(QString("%1 photos").arg(j + k + 1));
	    item->setObjectName(QString("res:photoalbum:%1").arg(j + k + 1));
	    connect(item, SIGNAL(clicked()), controller, SLOT(showAlbum()));
	    layout->addItem(item, j, k);

	    MAction* viewAction = new MAction ("View", item);
	    viewAction->setLocation(MAction::ObjectMenuLocation);
	    connect(viewAction, SIGNAL(triggered()), controller, SLOT(showAlbumFromObjectMenu()));
	    item->addAction(viewAction);

	    MAction* renameAction = new MAction ("Rename", item);
	    renameAction->setLocation(MAction::ObjectMenuLocation);
	    connect(renameAction, SIGNAL(triggered()), controller, SLOT(renameAlbum()));
	    item->addAction(renameAction);    

	    MAction* deleteAction = new MAction ("Delete", item);
	    deleteAction->setLocation(MAction::ObjectMenuLocation);
	    connect(deleteAction, SIGNAL(triggered()), controller, SLOT(deleteAlbum()));
	    item->addAction(deleteAction);
	}
    }
    */
}

M_REGISTER_VIEW_NEW(AlbumsView, Albums)
