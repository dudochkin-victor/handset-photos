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

#include <MAction>
#include <MButton>
#include <MLayout>
#include <MFlowLayoutPolicy>
#include <MWidgetController>
#include <QGraphicsLinearLayout>

#include "photos.h"
#include "photosview.h"
#include "photosmodel.h"
#include "appwindow.h"
#include "dynamicphotolist.h"

/*************************************************************************
 * PhotosView
 *************************************************************************/
PhotosView::PhotosView(Photos *controller) :
  MWidgetView(controller),
  controller(controller)
{
    }

PhotosView::~PhotosView()
{
    //delete photoList;
    //delete policy;
}

void PhotosView::updateData(const QList<const char*>& modifications)
{
    qDebug() << "PhotosView::updateData: " << modifications.count();

    MWidgetView::updateData(modifications);

    const char *member;
    foreach (member, modifications) {
	qDebug() << "updateData entry : " << member;
	if (member == PhotosModel::Query)
	{
	    updateView(model()->query());
	}
    }
}

void PhotosView::updateView(QString query)
{
    qDebug() << "PhotosView::updateView : " << query;

#if 0
    /*
     * Fill in some fake data to enable progress in developing the main UI
     */

    while(!layout->isEmpty())
	delete layout->takeAt(0);

    // Fill in some fake photo thumbnails
    for (int i=0; i<20; i++)
    {
	MButton *item = new MButton;
	item->setIconID("thumbnail");
        //item->setViewType(MButton::iconType);
        //item->setObjectName(QString("res:photo:%1").arg(i));
	connect(item, SIGNAL(clicked()), controller, SLOT(showPhoto()));
	policy->addItem(item);

	MAction* viewAction = new MAction ("View", item);
	viewAction->setLocation(MAction::ObjectMenuLocation);
	connect(viewAction, SIGNAL(triggered()), controller, SLOT(showPhotoFromObjectMenu()));
	item->addAction(viewAction);
	
	MAction* deleteAction = new MAction ("Delete", item);
	deleteAction->setLocation(MAction::ObjectMenuLocation);
	connect(deleteAction, SIGNAL(triggered()), controller, SLOT(deletePhoto()));
	item->addAction(deleteAction);
    }
#endif
}

M_REGISTER_VIEW_NEW(PhotosView, Photos)
