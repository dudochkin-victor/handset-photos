/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include <MButton>
#include <MPositionIndicator>
#include <QGraphicsLinearLayout>

#include "dynamicphotolist.h"
#include "searchresults.h"


SearchResults::SearchResults(MPProxyListModel *photosModel, MPProxyListModel *albumsModel, QGraphicsItem *parent) : MWidgetController(parent)
{
    topLayout = new QGraphicsLinearLayout(Qt::Vertical, this);
    topLayout->setSpacing(0);
    topLayout->setContentsMargins(0, 0, 0, 0);

    MWidgetController *tabBar = new MWidgetController(this);
    tabBarLayout = new QGraphicsLinearLayout(Qt::Horizontal, tabBar);
    tabBarLayout->setSpacing(0);
    tabBarLayout->setContentsMargins(0, 0, 0, 0);
	
	 //% "Photos"
    MButton *photosButton = new MButton(qtTrId("Photos"));
    photosButton->setObjectName("photosButtonInSearch");
    connect(photosButton, SIGNAL(clicked()), this, SLOT(onPhotoButtonClicked()));
	//% "Album"
    MButton *albumsButton = new MButton(qtTrId("Albums"));
    albumsButton->setObjectName("albumsButtonInSearch");
    connect(albumsButton, SIGNAL(clicked()), this, SLOT(onAlbumButtonClicked()));

    tabBarLayout->addItem(photosButton);
    tabBarLayout->addItem(albumsButton);
    //tabBarLayout->addStretch();

    topLayout->addItem(tabBar);

    MWidgetController *listArea = new MWidgetController(this);
    listAreaLayout = new QGraphicsLinearLayout(Qt::Vertical, listArea);
    listAreaLayout->setSpacing(0);
    listAreaLayout->setContentsMargins(0, 0, 0, 0);

    viewport = new MPannableViewport(listArea);
    viewport->setObjectName("viewportInSearch");
    viewport->positionIndicator()->hide();

    photosResultsList = new DynamicPhotoList(viewport);
    photosResultsList->setObjectName("photosResultsList");
    photosResultsList->setItemModel(photosModel);

    albumsResultsList = new DynamicAlbumList(viewport);
    albumsResultsList->setObjectName("albumsResultsList");
    qDebug() << "albumsResulssList(" << albumsResultsList<< ")xxxxxxx albumsModel:" << albumsModel;
    albumsResultsList->setItemModel(albumsModel);
    albumsResultsList->hide();

    viewport->setWidget(photosResultsList);

    listAreaLayout->addItem(viewport);

    topLayout->addItem(listArea);
}


SearchResults::~SearchResults()
{
}

void SearchResults::onPhotoButtonClicked()
{
    viewport->setWidget(photosResultsList);
    photosResultsList->show();
    albumsResultsList->hide();
}

void SearchResults::onAlbumButtonClicked()
{
    viewport->setWidget(albumsResultsList);
    albumsResultsList->show();
    photosResultsList->hide();
}
