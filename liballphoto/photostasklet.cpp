/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */


#include <QCryptographicHash>
#include <QDir>
#include <QDebug>
#include "photostasklet.h"
#include "mplistmodel.h"

PhotosTasklet::PhotosTasklet(DynamicPhotoList *list) : outstandingChecking(false)
{
    this->list = list;
}

PhotosTasklet::~PhotosTasklet()
{

}
	
QPixmap PhotosTasklet::squareQPixmapFromPath(QString path, qreal edgeR)
{
    QImage a = QImage(path);
    int edge = (int)edgeR;
/*    int w = a.width();
    int h = a.height();
    int edge = w;
    if (edge > h)
        edge = h; */
    QImage b = a.scaled(edge, edge, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    QImage c = b.copy(0, 0, edge, edge);
    return QPixmap::fromImage(c);
}

#if 0
void PhotosTasklet::checkThumbnailsGenerated()
{
    outstandingChecking = false;
    if (thumbnailsToBeGenerated.isEmpty())
        return;
    
    int cnt = thumbnailsToBeGenerated.count();
    QTime now;
    now.start();
    qDebug() << "PhotosTasklet::checkThumbnailsGenerated() time" << now << ":" << now.msec();
    while (cnt-- > 0) {
        ThumbnailCheckJob j = thumbnailsToBeGenerated.takeFirst();
        qDebug() << "PhotosTasklet::checkThumbnailsGenerated() duetime" << j.dueTime << "cnt:" << cnt;
        now.start();
        if (j.dueTime < now)
            continue;

        MPListItem *entry = static_cast<MPListItem *>(j.userData.value<void *>());
        QModelIndex index = j.row;
        MPListModel *photoModel = dynamic_cast<MPListModel *>(const_cast<QAbstractItemModel *>(index.model()));
        QFile f;
        f.setFileName(entry->thumbnailURI);
        if (f.exists()) {
            entry->thumbnail = new QPixmap(squareQPixmapFromPath(entry->thumbnailURI, AppWindow::instance()->photoList->cellCreator()->cellSize().toSize().width()));
            //entry->thumbnail = new QPixmap(squareQPixmapFromPath(entry->thumbnailURI, 112));
            entry->isLoaded = true;
            photoModel->thumbnailWasLoaded(index);
        } else {
            thumbnailsToBeGenerated << j;
        }
    }

    /*if(thumbnailsToBeGenerated.count() > 0)
        QTimer::singleShot(CHECK_THUMBNAIL_FILE_INTERVAL, this, SLOT(checkThumbnailsGenerated()));*/
}
#endif

QString PhotosTasklet::getThumbnailPath(const QString &uri)
{
    //use its Utf8 to perform md5
    QByteArray md5Result = QCryptographicHash::hash(uri.toUtf8(), QCryptographicHash::Md5);
    QString thumbnailPath = MPListModel::homePath + MPListModel::thumbnail_folder + QDir::separator() + md5Result.toHex() + MPListModel::thumbnail_suffix;
    return thumbnailPath;
}

void PhotosTasklet::processSingleJob(Job& j)
{
    QModelIndex index = j.row;
    MPListItem *entry = static_cast<MPListItem *>(j.userData.value<void *>());
    QAbstractItemModel *model = const_cast<QAbstractItemModel *>(index.model());
    MPListModel *photoModel = dynamic_cast<MPListModel *>(model);
    QByteArray md5Result;
#if 0
	if (!entry->isLoaded) {
        qDebug() << "processSingleJob: row " << index.row() << "thumbnail:" << QUrl::fromEncoded(entry->photoURI.toAscii()).path() << "entry@" << (void *)entry;
		entry->thumbnail = new QPixmap(squareQPixmapFromPath(QUrl::fromEncoded(entry->photoURI.toAscii()).path()));
        entry->isLoaded = true;
        photoModel->thumbnailWasLoaded(index);
	}
#else
    //generate thumbnailURI if it does not exist here
    if (entry->thumbnailURI.isEmpty()) {
#if 0
        QUrl url = QUrl::fromEncoded(entry->photoURI.toAscii());
        QString path = QString("file://") + url.path();
        //use its Utf8 to perform md5
        md5Result = QCryptographicHash::hash(path.toUtf8(), QCryptographicHash::Md5);
        entry->thumbnailURI = photoModel->homePath + photoModel->thumbnail_folder + QDir::separator() + md5Result.toHex() + photoModel->thumbnail_suffix;
        qDebug() << "entry " << entry << " " << path  << " " << "thumbnailURI generated is " << entry->thumbnailURI;
#else
        entry->thumbnailURI = getThumbnailPath(entry->photoURI);
#endif
    }

    //Some thumbnails may have not been created yet
    QFile f;
    f.setFileName(entry->thumbnailURI);
    if (f.exists()) {
        entry->thumbnail = new QPixmap(squareQPixmapFromPath(entry->thumbnailURI, list->cellCreator()->cellSize().toSize().width()));
        //entry->thumbnail = new QPixmap(squareQPixmapFromPath(entry->thumbnailURI, 112));
        entry->isLoaded = true;
        entry->showSpinner = false;
        QModelIndex next = index.sibling(index.row() + 1, 0);
        if (next.isValid()) {
            QVariant data = next.data(Qt::DisplayRole);
            MPListItem *next_entry = static_cast<MPListItem *>(data.value<void *>());
            if (!next_entry->isLoaded)
                next_entry->showSpinner = true;
            photoModel->thumbnailWasLoaded(next);
        }
        photoModel->thumbnailWasLoaded(index);
        QTimer::singleShot(0, this, SLOT(processJobQueue()));
    } else {
        QUrl url = QUrl::fromEncoded(entry->photoURI.toAscii());
        photoModel->addOutstandingThumbnailRequest(url.path());
        Thumbnailer::instance()->requestThumbnail(QString(("file://" + url.path())), entry->mimetype);
    }

#endif
}
