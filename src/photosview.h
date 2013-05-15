/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */


#ifndef PHOTOSVIEW_H
#define PHOTOSVIEW_H

#include <MExport>
#include <MWidgetView>
#include <QGraphicsLinearLayout>

#include "photosmodel.h"
#include "photosstyle.h"
#include "dynamicphotolist.h"
#include "appwindow.h"

class Photos;
class MButton;
class FeedsPage;
class MLayout;
class MFlowLayoutPolicy;

class M_EXPORT PhotosView : public MWidgetView
{
    Q_OBJECT
    M_VIEW(PhotosModel, PhotosStyle)

public:
    PhotosView(Photos *controller);
    virtual ~PhotosView();
    DynamicPhotoList *getList() const { return AppWindow::instance()->photoList;}

protected slots:
    virtual void updateData(const QList<const char*>& modifications);

private:
    void updateView(QString query);

    Photos *controller;
//    QGraphicsLinearLayout *layout;
//    MFlowLayoutPolicy *policy;
};

#endif // PHOTOSVIEW_H
