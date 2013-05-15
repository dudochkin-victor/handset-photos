/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */


#ifndef PHOTOSTRIPVIEW_H
#define PHOTOSTRIPVIEW_H

#include <MExport>
#include <MWidgetView>
#include <MPannableViewport>

#include "photostripmodel.h"
#include "photostripstyle.h"
#include "mplistmodel.h"
#include "stripitem.h"

class PhotoStrip;
class QGraphicsLinearLayout;
class QGraphicsWidget;
class PhotoStripPhysics;
class PhotoStripLayout;

class Viewport : public MPannableViewport
{
    Q_OBJECT
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    virtual void cancelEvent(MCancelEvent *event);

Q_SIGNALS:
    void mousePress();
private:

};

class M_EXPORT PhotoStripView : public MWidgetView
{
    Q_OBJECT
    M_VIEW(PhotoStripModel, PhotoStripStyle)

public:
    enum StripMode {
        OnNormal = 0,
        OnMultiTouch,
    };
    PhotoStripView(PhotoStrip *controller);
    virtual ~PhotoStripView();

    virtual void setGeometry (const QRectF &rect);
    virtual void applyStyle();
    void focusAreaLoad(int focus);
    int getFocusedWidget();

public slots:
    void mousePress();

protected slots:
    virtual void updateData(const QList<const char*>& modifications);
    void sizePosChanged (const QSizeF &viewportSize, const QRectF &pannedRange, const QPointF &pannedPos);
    void snapIndexChanged(int newPosition);
    void panningStopped();
    void showNextImage();
    void modelRowChanged(const QModelIndex &index, int start, int end);

protected:
    virtual void pinchGestureEvent(QGestureEvent *event, QPinchGesture *gesture);

private:
    void updateView(QString query);
    void updateView(QModelIndex stripIndex);
    void setStripSmall(bool on);

    void updateSnapInterval();
    void startSlideShow();

    PhotoStrip *controller;
    MWidget* layoutWidget;
    MPannableViewport *viewport;

    /*! Layouts */
    QGraphicsLinearLayout *mainLayout;
    QGraphicsWidget* pannedWidget;
    PhotoStripLayout* pannedLayout;    

    /*! The integrator for the pannable viewport, controls how the filmstrip buttons move */
    PhotoStripPhysics* physics;

    /*! The current focused filmstrip button */
    int focusedWidget;

    /* all items list */
    QMap<int, StripItem*> widgets;

    QAbstractItemModel *currentModel;

    static const int BUFFERSIZE;
    static const int ITEM_SPACING = 10;
    static const int SLIDESHOW_INTERVAL;
    bool _isPositionChanged;
    int _PositionValue;
    QTimer *timer;
    bool isModelRowChanged;
    StripMode mode;
    QPointF center;
};


#endif // PHOTOSTRIPVIEW_H
