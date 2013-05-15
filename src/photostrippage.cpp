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
#include <QTapAndHoldGesture>
#include "photostrippage.h"

PhotoStripPage::PhotoStripPage(QGraphicsItem *parent) : MApplicationPage(parent)
{
}

PhotoStripPage::~PhotoStripPage()
{
}

void PhotoStripPage::tapAndHoldGestureEvent(QGestureEvent *event, QTapAndHoldGesture *gesture)
{
    if (gesture->state() == Qt::GestureFinished) {
        QGraphicsItem *mouseGrab = scene()->mouseGrabberItem();
        mouseGrab->ungrabMouse();
    }
    MSceneWindow::tapAndHoldGestureEvent(event, gesture);
}
