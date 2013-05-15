/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */


#include "stripitem.h"
#include "appwindow.h"
#include "photostripview.h"
#include "stripimageview.h"
#include <QtGui>
#include <MAction>
#include <MImageWidget>

QPixmap* StripItem::defaultPic = NULL;
StripItem::StripItem()
{
}

StripItem::StripItem(QString uri, PhotoStripView *view) :
    stripURI(uri), isLoaded(false), stripView(view), photoListDeleteConfirm(NULL)
{
    
    qDebug() << "StripItem uri" << uri;
    if (!defaultPic)
        defaultPic = const_cast<QPixmap *>(MTheme::pixmap("defaultThumbnail"));

    stripImg = new MImageWidget();
    stripImgView = new StripImageView(stripImg);
    stripImg->setView(stripImgView);
    //stripImg->setObjectName("PhotoStripImage");
    
    stripImg->setPixmap(*defaultPic);

 /*   MAction* copyAction = new MAction ("Copy", stripImg);
    copyAction->setLocation(MAction::ObjectMenuLocation);
    connect(copyAction, SIGNAL(triggered()), this, SLOT(copyPhoto()));
    stripImg->addAction(copyAction); */
    
    //% "Rotate Clockwise"
    MAction* rotatecwAction = new MAction (qtTrId("Rotate Clockwise"), stripImg);
    rotatecwAction->setLocation(MAction::ObjectMenuLocation);
    connect(rotatecwAction, SIGNAL(triggered()), this, SLOT(rotateCW()));
    stripImg->addAction(rotatecwAction);

    //% "Rotate Counter Clockwise"
    MAction* rotateccwAction = new MAction (qtTrId("Rotate Counter Clockwise"), stripImg);
    rotateccwAction->setLocation(MAction::ObjectMenuLocation);
    connect(rotateccwAction, SIGNAL(triggered()), this, SLOT(rotateCCW()));
    stripImg->addAction(rotateccwAction);

    //% "Delete"
    MAction* deleteAction = new MAction (qtTrId("Delete"), stripImg);
    deleteAction->setLocation(MAction::ObjectMenuLocation);
    connect(deleteAction, SIGNAL(triggered()), this, SLOT(deletePhoto()));
    stripImg->addAction(deleteAction);
}


StripItem::~StripItem()
{
    unload();
    delete stripImg;
    if (photoListDeleteConfirm)
        delete photoListDeleteConfirm;
}

MImageWidget* StripItem::StripImg()
{
    return stripImg;
}

StripImageView* StripItem::StripImgView()
{
    return stripImgView;
}

void StripItem::load()
{
    if (isLoaded)
        return;
    QUrl url = QUrl::fromEncoded(stripURI.toAscii());
    QSize windowSize = (AppWindow::instance())->visibleSceneSize();
    qDebug() << "StripItem::load() url:" << url.path() << "windowSize:" << windowSize;
    QPixmap *img = new QPixmap(url.path());
    QPixmap img2 = img->scaled(windowSize, Qt::KeepAspectRatio);
    stripImg->setPixmap(img2);
    delete img;
    isLoaded = true;
}

void StripItem::unload()
{
    if (!isLoaded)
        return;
    stripImg->setPixmap(*defaultPic);
    isLoaded = false;
}

void StripItem::copyPhoto()
{
    QUrl url = QUrl::fromEncoded(stripURI.toAscii());
    QString path = url.path();
    QStringList list = path.split("/");
    QString filename = list.last();

    QString targetName = "copy_of_" + filename;
    list[list.count()-1] = targetName;
    QString targetString = list.join("/");
    int n = 2;
    while (QFile::exists(targetString)) {
        targetName = "copy_" + QString::number(n) + "_of_" + filename;
        list[list.count()-1] = targetName;
        targetString = list.join("/");
        n++;
    }
    QFile::copy(path, targetString);
}

void StripItem::rotateCW()
{
    AppWindow::instance()->hideOverlay(true);
    const QPixmap *img = stripImg->pixmap();
    QPixmap rotatedImg(img->height(), img->width());
    rotatedImg.fill();
    QPainter p;
    p.begin(&rotatedImg);
    p.save();
    p.rotate(90);
    p.drawPixmap(0, -img->height(), *img);
    p.restore();
    p.end();

    stripImg->setPixmap(rotatedImg);
}

void StripItem::rotateCCW()
{
    AppWindow::instance()->hideOverlay(true);
    const QPixmap *img = stripImg->pixmap();
    QPixmap rotatedImg(img->height(), img->width());
    rotatedImg.fill();
    QPainter p;
    p.begin(&rotatedImg);
    p.save();
    p.rotate(270);
    p.drawPixmap(-img->width(), 0, *img);
    p.restore();
    p.end();

    stripImg->setPixmap(rotatedImg);
}

void StripItem::deletePhoto()
{
    /* delete photo */
    QUrl url = QUrl::fromEncoded(stripURI.toAscii());
    QString path = url.path();
    if (!photoListDeleteConfirm)
        photoListDeleteConfirm = new PhotoListDeleteConfirm();
    photoListDeleteConfirm->setFile(path);
    //AppWindow::instance()->deletePhoto("Delete Photo2", "Are you sure to delete " + path + "?", photoListDeleteConfirm);
    //% "Are you sure you want to delete this photo"
    AppWindow::instance()->deletePhoto("", qtTrId("Are you sure you want to delete this photo") + "?", photoListDeleteConfirm);
}

void StripItem::scaleFactorChange(qreal factor, QPointF center)
{
    QRectF rec = stripImg->crop();
    stripImg->setCrop(QRectF(center, rec.size()));
    stripImg->setZoomFactor(factor);
    stripImg->update();
}
