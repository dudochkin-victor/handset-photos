/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */


#ifndef ALBUMSVIEW_H
#define ALBUMSVIEW_H

#include <MExport>
#include <MWidgetView>

#include "albumsmodel.h"
#include "albumsstyle.h"

class Albums;
class MButton;
class FeedsPage;
class QGraphicsGridLayout;

class M_EXPORT AlbumsView : public MWidgetView
{
    Q_OBJECT
    M_VIEW(AlbumsModel, AlbumsStyle)

public:
    AlbumsView(Albums *controller);
    virtual ~AlbumsView();

protected slots:
    virtual void updateData(const QList<const char*>& modifications);

private:
    void updateView(QString query);

    QGraphicsGridLayout *layout;    
    Albums *controller;
};

#endif // ALBUMSVIEW_H
