/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
#ifndef PHOTOSTRIPPAGE_H
#define PHOTOSTRIPPAGE_H
#include <MApplicationPage>

class PhotoStripPage : public MApplicationPage
{
    Q_OBJECT
public:
    PhotoStripPage(QGraphicsItem *parent = 0);
    virtual ~PhotoStripPage();

protected:
    //! \reimp
    virtual void tapAndHoldGestureEvent(QGestureEvent *event, QTapAndHoldGesture *gesture);
    //! \reimp_end
};

#endif // PHOTOSTRIPPAGE_H
