/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include "stripimageview.h"
#include <QtGui>

StripImageView::StripImageView(MImageWidget *controller) :
    MImageWidgetView(controller),
    controller(controller)
{
}

void StripImageView::setStyleSmall(bool on)
{
    if (on)
        style().setModeSmall();
    else
        style().setModeDefault();
    update();
}

StripImageView::~StripImageView()
{
}

void StripImageView::drawContents(QPainter *painter, const QStyleOptionGraphicsItem *option) const
{
    Q_UNUSED(option);

    // no image, return
    QSizeF imgSize = controller->imageSize();
    if (imgSize.isEmpty()) return;

    // the source image section size will be used finally
    QSizeF sourceSize = imgSize;

    // get target size, bounded by widget size
    QSizeF widgetSize = size();
    QSizeF targetSize = widgetSize;
    QSizeF t;
    // get the image display size
    qreal fx, fy;
    controller->zoomFactor(&fx, &fy);

    t.setWidth(imgSize.width()*fx);
    t.setHeight(imgSize.height()*fy);
    // update sourceSize for crop section by compare with targetSize, simulate zoom effect
    qreal value;
    if (t.width() > targetSize.width()) {
        value = sourceSize.width();
        sourceSize.setWidth(qRound(value * targetSize.width() / t.width()));
    }
    if (t.height() > targetSize.height()) {
        value = sourceSize.height();
        sourceSize.setHeight(qRound(value * targetSize.height() / t.height()));
    }

    // limited by target size
    t = targetSize.boundedTo(t);

    // protection codes
    if (sourceSize.width() < 1.0)
        sourceSize.setWidth(1.0);
    if (sourceSize.height() < 1.0)
        sourceSize.setHeight(1.0);

    // get values from controller
    const QPixmap *pixmap = controller->pixmap();

    // notice: no border on the cropped edge
    QSizeF originSize = pixmap->size();

    // calculate the rectangle of draw
    qreal dx = (widgetSize.width() - t.width()) / 2.0;
    qreal dy = (widgetSize.height() - t.height()) / 2.0;

    // calculate draw rect
    QRectF drawRect, sourceRect, border;
    drawRect.setRect(dx, dy, t.width(), t.height());

    // draw borders outside of target
    // draw image
    QPointF topLeft = controller->crop().topLeft();

    dx = (originSize.width() - sourceSize.width()) / 2.0;
    dy = (originSize.height() - sourceSize.height()) / 2.0;

    if ((fx > 1 || fx > 1) && topLeft != QPointF(0, 0)) {
        dx += (topLeft.x() - targetSize.width() / 2);
        dy += (topLeft.y() - targetSize.height() / 2);
    }

    if (dx <= 0)
        dx = 0;
    if (dy <= 0)
        dy = 0;
    if (dx >= (originSize.width() - sourceSize.width()))
        dx = originSize.width() - sourceSize.width();
    if (dy >= (originSize.height() - sourceSize.height()))
        dy = originSize.height() - sourceSize.height();

    sourceRect = QRectF(dx, dy, sourceSize.width(), sourceSize.height());

    painter->drawPixmap(drawRect, *pixmap, sourceRect);
}

