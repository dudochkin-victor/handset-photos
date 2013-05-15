/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */


#ifndef STRIPITEM_H
#define STRIPITEM_H

#include <QtGui>

#include "photolistdeleteconfirm.h"

class MImageWidget;
class StripImageView;
class PhotoStripView;



class StripItem : public QObject
{
Q_OBJECT

public:
    StripItem(QString uri, PhotoStripView *view);
    virtual ~StripItem();
    MImageWidget* StripImg();
    StripImageView* StripImgView();
    void load();
    void unload();
    void scaleFactorChange(qreal factor, QPointF center = QPointF());

public slots:
    void copyPhoto();
    void rotateCW();
    void rotateCCW();
    void deletePhoto();

private:
    explicit StripItem(); // disable default constructor
    MImageWidget *stripImg;
    StripImageView *stripImgView;
    QString stripURI;
    bool isLoaded;
    static QPixmap *defaultPic;
    PhotoStripView *stripView;
    PhotoListDeleteConfirm *photoListDeleteConfirm;
};

#endif // STRIPITEM_H
