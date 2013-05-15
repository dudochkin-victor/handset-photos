/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */


#include "photostriplayout.h"
#include <MWidget>

PhotoStripLayout::PhotoStripLayout()
{
    m_spacing = 0;
    m_count = 0;
}

PhotoStripLayout::~PhotoStripLayout()
{
    clear();
}

int PhotoStripLayout::count() const
{
    return m_count;
}

void PhotoStripLayout::setCount(int count)
{
    m_count = count;
}

qreal PhotoStripLayout::spacing() const
{
    return m_spacing;
}

void PhotoStripLayout::setSpacing(qreal spacing)
{
    m_spacing = spacing;
}

void PhotoStripLayout::setGeometry(const QRectF &geom)
{
	qDebug() << "PhotoStripLayout::setGeometry" << geom;
    QGraphicsLayout::setGeometry(geom);
    if (m_count <= 0)
        return;

    qreal left, top, right, bottom;
    getContentsMargins(&left, &top, &right, &bottom);

    QRectF rect = geometry();
    qreal width = (rect.width() - left - right - spacing() * (m_count - 1)) / m_count;
    qreal height = rect.height() - top - bottom;

    QSizeF pref = QSizeF(width, height);
    QMapIterator<int, QGraphicsLayoutItem*> i(m_items);
    while (i.hasNext()) {
        i.next();
        int index = i.key();
        QGraphicsLayoutItem *item = i.value();
        QPointF point = QPointF(index * (width + spacing()) + left, top);
        item->setGeometry( QRectF( point, pref) );
    }
}

QGraphicsLayoutItem *PhotoStripLayout::itemAt(int index) const
{
    return m_items.value(index);
}

void PhotoStripLayout::removeAt(int index)
{
    if (m_items.contains(index))
        m_items.remove(index);
    invalidate();
}

void PhotoStripLayout::clear()
{
    m_items.clear();
    invalidate();
}

void PhotoStripLayout::addItem(QGraphicsLayoutItem *item)
{
	insertItem(m_count++, item);
}

void PhotoStripLayout::insertItem(int index, QGraphicsLayoutItem *item)
{
	qDebug() <<"PhotoStripLayout::insertItem:" << index;

    if (!item) {
        qWarning("PhotoStripLayout::insertItem: cannot insert null item");
        return;
    }
    if (item == this) {
        qWarning("PhotoStripLayout::insertItem: cannot insert itself");
        return;
    }

    if (m_items.contains(index)) {
        qDebug() << "PhotoStripLayout::insertItem : duplicate index";
        m_items.remove(index);
    }
    addChildLayoutItem(item);
    item->setParentLayoutItem(this);

    m_items.insert(index, item);
    m_count = qMax(m_count, index+1);
    invalidate();
}

QSizeF PhotoStripLayout::calculateSize(Qt::SizeHint which) const
{
    qreal targetWidth = 0.0;
    qreal targetHeight = 0.0;

    QMapIterator<int, QGraphicsLayoutItem*> i(m_items);
    if (i.hasNext()) {
        i.next();
        QGraphicsLayoutItem *item = i.value();
        QSizeF size = item->effectiveSizeHint(which);
        targetWidth = size.width() * m_count;
        if (m_count > 1)
            targetWidth += spacing() * (m_count - 1);
        targetHeight = size.height();
    }
    return QSizeF(targetWidth, targetHeight);
}

QSizeF PhotoStripLayout::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
{
    QSizeF size = constraint;
    switch (which) {
    case Qt::PreferredSize:
        size = calculateSize(which);
        break;
    case Qt::MinimumSize:
        size = calculateSize(which);
        break;
    case Qt::MaximumSize:
        size = calculateSize(which);
        break;
    default:
        break;
    }
    
    qDebug() << "sizeHint" << which << size;
    return size;
}

