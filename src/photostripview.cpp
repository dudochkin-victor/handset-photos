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
#include <QGraphicsLinearLayout>
#include <QGraphicsWidget>
#include <QPixmap>
#include <QSize>

#include <MAction>
#include <MImageWidget>
#include <MPannableViewport>
#include <MApplicationPage>
#include <MPositionIndicator>

#include "photostrip.h"
#include "photostripview.h"
#include "photostripmodel.h"
#include "appwindow.h"
#include "photostripphysics.h"
#include "mplistmodel.h"
#include "stripimageview.h"
#include "photostriplayout.h"
/*************************************************************************
 * PhotoStripView
 *************************************************************************/

void Viewport::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
    emit mousePress();
}

void Viewport::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug() << "123456: Viewport::mouseReleaseEvent overlayvisible" <<  AppWindow::instance()->isOverlayShown();
    if (!AppWindow::instance()->isOverlayShown()) {
        AppWindow::instance()->showOverlay();
        AppWindow::instance()->autoHideTimer.start(3000);
    } else {
        AppWindow::instance()->autoHideTimer.stop();
        AppWindow::instance()->hideOverlay(true);
    }

    MWidgetController::mouseReleaseEvent(event);
}

void Viewport::cancelEvent(MCancelEvent *event)
{
    Q_UNUSED(event);
    /* we don't want to ungrab mouse, keep pan gesture */
    /* overwrite MPannableWidget::cancelEvent() */
    /* fix bug #3532 */
}



const int PhotoStripView::BUFFERSIZE = 2;
const int PhotoStripView::SLIDESHOW_INTERVAL = 3000; // ms

PhotoStripView::PhotoStripView(PhotoStrip *controller) :
    MWidgetView(controller),
    controller(controller),
    viewport(new Viewport),
    mainLayout(new QGraphicsLinearLayout(Qt::Horizontal)),
    pannedWidget(new QGraphicsWidget),
    focusedWidget(-1), _isPositionChanged(false),
    isModelRowChanged(false), currentModel(0),
    mode(PhotoStripView::OnNormal)
{
    /* controller initial setting */
    controller->setContentsMargins(0, 0, 0, 0);
    controller->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    /* pannable viewport initial setting */
    viewport->setObjectName("PhotoStripDetailsViewport");
    viewport->setPanDirection(Qt::Horizontal);
    viewport->setContentsMargins(0, 0, 0, 0);
    viewport->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    viewport->setAutoRange(false);

    /* the layout of viewport initial setting */
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addItem(viewport);

    controller->setLayout(mainLayout);

    /* pannedWidget initial setting */
    pannedWidget->setContentsMargins(0, 0, 0, 0);
    pannedWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    /* pannedLayout initial setting */
    pannedLayout = new PhotoStripLayout();
    pannedWidget->setLayout(pannedLayout);
    pannedLayout->setSpacing(ITEM_SPACING);
    pannedLayout->setContentsMargins(0, 0, 0, 0);

    connect(viewport, SIGNAL(sizePosChanged(const QSizeF &, const QRectF &, const QPointF &)),
            this, SLOT(sizePosChanged(const QSizeF &, const QRectF &, const QPointF &)));

    viewport->setWidget(pannedWidget);
    viewport->positionIndicator()->hide();

    physics = new PhotoStripPhysics(this);
    connect(physics, SIGNAL(snapIndexChanged(int)), this, SLOT(snapIndexChanged(int)));
    viewport->setPhysics(physics);

    connect(viewport, SIGNAL(panningStopped()), this, SLOT(panningStopped()));
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(showNextImage()));
    timer->setInterval(SLIDESHOW_INTERVAL);
    connect(viewport, SIGNAL(mousePress()), this, SLOT(mousePress()));

}

void PhotoStripView::mousePress()
{
    timer->stop();
}

void PhotoStripView::pinchGestureEvent(QGestureEvent *event, QPinchGesture *gesture)
{
    qDebug() << "PhotoStripView::pinchGestureEvent";
    Qt::GestureState state = gesture->state();
    int width = (AppWindow::instance())->visibleSceneSize().width() + ITEM_SPACING;
    QPinchGesture::ChangeFlags flags = gesture->changeFlags();
    switch (state) {
    case Qt::GestureStarted:
        AppWindow::instance()->hideOverlay(true);
        mode = PhotoStripView::OnMultiTouch;
        viewport->setEnabled(false);
        viewport->setPosition(QPointF(focusedWidget * width, 0));
        center = gesture->centerPoint();
        break;
    case Qt::GestureFinished:
    case Qt::GestureCanceled:
        mode = PhotoStripView::OnNormal;
        viewport->setEnabled(true);
        break;
    default:
        break;
    }

    if (flags &  QPinchGesture::ScaleFactorChanged) {
        qreal factor = gesture->scaleFactor();
        // make sure factor is not NaN
        if (factor == factor)
        {
            widgets[focusedWidget]->scaleFactorChange(factor, center);
        }
    }

    event->accept(gesture);
}


void PhotoStripView::showNextImage()
{
    int max = viewport->range().width();
    int width = (AppWindow::instance())->visibleSceneSize().width() + ITEM_SPACING;
    QPointF point = viewport->position();
    if (point.x() + width >= max) {
        timer->stop();
        qDebug() << "PhotoStripView::showNextImage() calling switchPageBack()";
        (AppWindow::instance())->switchPageBack();
    } else {
        point.setX(point.x() + width);
        physics->setEnabled(false);
        viewport->setPosition(point);
        physics->setEnabled(true);
        updateSnapInterval();
    }
}

PhotoStripView::~PhotoStripView()
{
    delete viewport;
    delete mainLayout;
    delete pannedWidget;
}

void PhotoStripView::updateData(const QList<const char*>& modifications)
{
    qDebug() << "PhotoStripView::updateData: " << modifications.count();

    MWidgetView::updateData(modifications);

    const char *member;
    foreach (member, modifications) {
	    qDebug() << "PhotoStripView::updateData: entry : " << member;
        if (member == PhotoStripModel::StripIndex)
            updateView(model()->stripIndex());

        if (member == PhotoStripModel::SlideShow) {
            updateView(model()->stripIndex());
            startSlideShow();
        }

        if (mode == PhotoStripView::OnNormal && member == PhotoStripModel::StripSmall)
            setStripSmall(model()->stripSmall());
    }
}

void PhotoStripView::modelRowChanged(const QModelIndex &index, int start, int end)
{
    Q_UNUSED(index);
    Q_UNUSED(end);

    isModelRowChanged = true;
    int count = currentModel->rowCount();

    /* return to show all photos if no pictures in single photo view */
    if (count == 0) {
        qDebug() << "PhotoStripView::modelRowChanged calling showAllPhotos";
        (AppWindow::instance())->showAllPhotos();
        return;
    }

    /* adjust photo */
    int next = 0;    
    if (start < count) {
        next = start;
    }
    else {
        next = start - 1;
    }
        
    updateView(currentModel->index(next, 0));
    
}

void PhotoStripView::updateView(QModelIndex stripIndex)
{
    /* model is changing */
    qDebug() << "PhotoStripView::updateView" << stripIndex.row();
    QAbstractItemModel *model = 
        const_cast<QAbstractItemModel *>(stripIndex.model());

    /* return if model is null */
    if(!model)
        return;

    int count = model->rowCount();
    if (currentModel != model || isModelRowChanged == true) {
        isModelRowChanged = false;
        
        if (currentModel != model)
        {
            currentModel = model;
            connect(currentModel, SIGNAL(rowsRemoved(const QModelIndex &, int, int)),
                this, SLOT(modelRowChanged(const QModelIndex &, int, int)));
        }

        /* remove all items from pannedLayout while model is changing */ 
        pannedLayout->clear();

        /* delete all items */
        QList<StripItem *> itemList = widgets.values();
        while (!itemList.isEmpty()) {
            StripItem *item = static_cast<StripItem *>(itemList.first());
            itemList.removeFirst();
            qDebug() << "item is " << item;
            delete item;
            qDebug() << "after delet item\n";
        }
        widgets.clear();

        pannedLayout->setCount(count);
    }

    /* set viewport range */
    viewport->setRange(QRectF(0, 0, 0, 0));
    if (count) {
        int w = (AppWindow::instance())->visibleSceneSize().width();
        int h = (AppWindow::instance())->visibleSceneSize().height();
        QRectF rec = QRectF(0, 0, (w * count) + ITEM_SPACING * (count - 1), h);
        viewport->setRange(rec);
    }

    /* load real image in focus area */
    focusedWidget = stripIndex.row();

    int min = focusedWidget - 1;
    int max = focusedWidget + 1;
    if (min < 0)
        min = 0;
    if (max > count - 1)
        max = count - 1;

    for (int i = min; i <= max; i++) {
        if ( !widgets.contains(i) ) {
            QVariant data = currentModel->data(currentModel->index(i, 0), 
                                            Qt::DisplayRole);
            MPListItem *item = static_cast<MPListItem *>(data.value<void *>());
            QString uri = QUrl(item->photoURI).toLocalFile();
            qDebug() << "2222: uri:" << uri;
            StripItem *stripItem;
            stripItem = new StripItem(uri, this); 
            widgets.insert(i, stripItem);
            pannedLayout->insertItem(i, stripItem->StripImg());
        }

        StripItem *stripItem = widgets[i];
        stripItem->load();
    }

    int width = (AppWindow::instance())->visibleSceneSize().width() + ITEM_SPACING;
    viewport->setEnabled(false);
    physics->setEnabled(false);
    viewport->setPosition(QPointF(focusedWidget * width, 0));
    physics->setEnabled(true);
    viewport->setEnabled(true);
    QVariant focusedData = currentModel->data(currentModel->index(focusedWidget, 0),
                                                            Qt::DisplayRole);
    MPListItem *focusedItem = static_cast<MPListItem *>(focusedData.value<void *>());
    AppWindow::instance()->updateCurrentPhoto(QUrl(focusedItem->photoURI).toLocalFile(), focusedItem->urn);

    updateSnapInterval();
}

void PhotoStripView::setStripSmall(bool on)
{
    QList<StripItem *> itemList = widgets.values();
    while (!itemList.isEmpty()) {
        StripItem *item = static_cast<StripItem *>(itemList.first());
        itemList.removeFirst();
        item->StripImgView()->setStyleSmall(on);
    }
}

void PhotoStripView::startSlideShow()
{
    timer->start();
}

void PhotoStripView::setGeometry(const QRectF &rect)
{
    qDebug() << "PhotoStrip::setGeometry : " << rect;
    MWidgetView::setGeometry(rect);
    updateSnapInterval();
}

void PhotoStripView::applyStyle()
{
    qDebug() << "PhotoStripView::applyStyle()";
    MWidgetView::applyStyle();
    updateSnapInterval();
}

void PhotoStripView::sizePosChanged(const QSizeF &viewportSize, 
				    const QRectF &pannedRange, 
				    const QPointF &pannedPos) 
{
    Q_UNUSED(viewportSize);
    Q_UNUSED(pannedRange);
    Q_UNUSED(pannedPos);
}

int PhotoStripView::getFocusedWidget()
{
    return focusedWidget;
}

void PhotoStripView::focusAreaLoad(int focus)
{
    /* make sure changing or not */
    if (focus == focusedWidget)
        return;
    if (!currentModel)
        return;

    qDebug() << "PhotoStripView::focusAreaLoad : " << focus;
    
    bool isLeft = true;
    if (focus > focusedWidget)
        isLeft = false;

    focusedWidget = focus;
    int count = currentModel->rowCount();

    /* load real image in focus area */
    int min = focusedWidget - BUFFERSIZE;
    int max = focusedWidget + BUFFERSIZE;
    if (min < 0)
        min = 0;
    if (max > count - 1)
        max = count - 1;

    for (int i = min; i <= max; i++) {
        if ( !widgets.contains(i) ) {
            QVariant data = currentModel->data(currentModel->index(i, 0), 
                                            Qt::DisplayRole);
            MPListItem *item = static_cast<MPListItem *>(data.value<void *>());
            QString uri = (item->photoURI).mid(7);
            StripItem *stripItem;
            stripItem = new StripItem(uri, this); 
            widgets.insert(i, stripItem);
            pannedLayout->insertItem(i, stripItem->StripImg());
        }

        StripItem *stripItem = widgets[i];
        stripItem->load();
    }

    if (isLeft) {
        if (max + 1 <= count - 1) {
            if (widgets.contains(max+1)) {
                StripItem *stripItem = widgets[max+1];
                stripItem->unload();
            }
        }
        if (widgets.contains(focusedWidget+1))
            widgets[focusedWidget+1]->scaleFactorChange(1);
    } else {
        if (min - 1 >= 0) {
            if (widgets.contains(min-1)) {
                StripItem *stripItem = widgets[min-1];
                stripItem->unload();
            }
        }
        if (widgets.contains(focusedWidget-1))
            widgets[focusedWidget-1]->scaleFactorChange(1);
    }

    MWidget* w = widgets[focusedWidget]->StripImg();
    w->setFocus();
    QVariant focusedData = currentModel->data(currentModel->index(focusedWidget, 0),
                                                            Qt::DisplayRole);
    MPListItem *focusedItem = static_cast<MPListItem *>(focusedData.value<void *>());
    AppWindow::instance()->updateCurrentPhoto(QUrl(focusedItem->photoURI).toLocalFile(), focusedItem->urn);
}

void PhotoStripView::snapIndexChanged(int newPosition)
{
    _isPositionChanged = true;
    _PositionValue = newPosition;
}

void PhotoStripView::panningStopped()
{
    if (_isPositionChanged) {
        _isPositionChanged = false;
        focusAreaLoad(_PositionValue);
    }
}

void PhotoStripView::updateSnapInterval()
{
    qDebug() << "PhotoStripView::updateSnapInterval : " << style()->snapInterval();

    int snap = style()->snapInterval();
    if (snap == -1) {
        snap = (AppWindow::instance())->visibleSceneSize().width() + ITEM_SPACING;
    }
    if (snap != physics->snapInterval()) {
        physics->setEnabled(false);
        physics->setSnapInterval(snap);
        if(focusedWidget != -1)
            viewport->setPosition(QPointF(focusedWidget * snap, 0));
        physics->setEnabled(true);
    }
}


M_REGISTER_VIEW_NEW(PhotoStripView, PhotoStrip)
