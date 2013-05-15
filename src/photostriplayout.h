/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */


#ifndef PHOTOSTRIPLAYOUT_H
#define PHOTOSTRIPLAYOUT_H

#include <QtGui>

class PhotoStripLayout : public QGraphicsLayout
{
public:
    PhotoStripLayout();
    virtual ~PhotoStripLayout();
    void setGeometry(const QRectF &geom);
    int count() const;
    void setCount(int count);
    void addItem(QGraphicsLayoutItem *item);
    void insertItem(int index, QGraphicsLayoutItem *item);
    QGraphicsLayoutItem *itemAt(int index) const;
    void removeAt(int index);
    void clear();
    qreal spacing() const;
    void setSpacing(qreal spacing);
    QSizeF sizeHint(Qt::SizeHint which, const QSizeF & constraint = QSizeF()) const;

private:
    QMap<int, QGraphicsLayoutItem*> m_items;
    qreal m_spacing;
    int m_count;

    QSizeF calculateSize(Qt::SizeHint which) const;
};

#endif // PHOTOSTRIPLAYOUT_H
