/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef MCONTENTITEMEX_H
#define MCONTENTITEMEX_H

#include <MContentItem>
#include <QModelIndex>

class MContentItemEx : public MContentItem
{
public:

    MContentItemEx(MContentItem::ContentItemStyle itemStyle = MContentItem::TwoIconsTwoWidgets, QGraphicsItem *parent = 0, int _id = 0)
        :   MContentItem(itemStyle, parent), id(_id)
    {
    }

    virtual ~MContentItemEx(){};


    QModelIndex index;  // the index of this item in some model
    int id;

    void showSpinner();
    void hideSpinner();

public slots:
    void setPixmap(const QPixmap &pixmap);

};

#endif // MCONTENTITEMEX_H
