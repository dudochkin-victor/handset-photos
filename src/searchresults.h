/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef SEARCH_RESULTS_H
#define SEARCH_RESULTS_H
#include <MWidgetController>
#include <QGraphicsItem>
#include <QGraphicsLinearLayout>
#include <MPannableViewport>

#include "dynamicphotolist.h"
#include "dynamicalbumlist.h"
#include "mpproxylistmodel.h"

class SearchResults : public MWidgetController {
    Q_OBJECT
public:
    SearchResults(MPProxyListModel *photoModel, MPProxyListModel *albumModel, QGraphicsItem *parent = 0);
    virtual ~SearchResults();

public slots:
    void onPhotoButtonClicked();
    void onAlbumButtonClicked();

private:
    QGraphicsLinearLayout *topLayout;
    QGraphicsLinearLayout *tabBarLayout;
    QGraphicsLinearLayout *listAreaLayout;
    DynamicPhotoList *photosResultsList;
    DynamicAlbumList *albumsResultsList;
    MPannableViewport *viewport;
};

#endif
