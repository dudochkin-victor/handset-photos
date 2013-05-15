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

#include "photostrip.h"
#include "appwindow.h"

PhotoStrip::PhotoStrip(QGraphicsItem *parent) :
  MWidgetController(new PhotoStripModel(), parent)
{
    setActive(true);
    grabGesture(Qt::PinchGesture);
}

PhotoStrip::~PhotoStrip()
{
}

void PhotoStrip::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug() << "PhotoStrip::mouseReleaseEvent";
    MWidgetController::mouseReleaseEvent(event);
}

