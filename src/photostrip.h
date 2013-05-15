/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */


#ifndef PHOTOSTRIP_H
#define PHOTOSTRIP_H

#include <MWidgetController>
#include "photostripview.h"
#include "photostripmodel.h"

class M_EXPORT PhotoStrip : public MWidgetController
{
    Q_OBJECT
    M_CONTROLLER(PhotoStrip);

public:
    PhotoStrip(QGraphicsItem *parent = 0);
    virtual ~PhotoStrip();

public Q_SLOTS:
    void setStripIndex(QModelIndex stripIndex) {
        model()->setStripIndex(stripIndex);
    };
    void setSlideShow(bool on) {
        model()->setSlideShow(on);
    };
    void setStripSmall(bool on) {
        model()->setStripSmall(on);
    };

protected:
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
    friend class PhotoStripView;
};

#endif // PHOTOSTRIP_H
