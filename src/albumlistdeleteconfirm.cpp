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
#include <QFile>

#include "albumlistdeleteconfirm.h"
#include "searchengine.h"
#include "appwindow.h"

void AlbumListDeleteConfirm::onAccepted()
{
    qDebug() << "User accepts ";
    QVector<QStringList> result;
    MPListModel *model = dynamic_cast<MPListModel *>(AppWindow::instance()->albumList->itemModel());
    model->removeAlbum(index);
    SearchEngine::getAlbumPhotos(album, result);
    for (int i = 0; i < result.count(); i++) {
        QUrl url(result[i][0]);
        QString filePath = url.toLocalFile();
        QString urn = result[i][2];
        AppWindow::instance()->photoRemoveBlacklist << urn;
        qDebug() << "12345: " << filePath;
        QFile::remove(filePath);
    }
}

void AlbumListDeleteConfirm::setAlbum(const QString &albumToBeDeleted, const QModelIndex &itemIndex)
{
    album = albumToBeDeleted;
    index = itemIndex;
}

void AlbumListDeleteConfirm::onRejected()
{
    qDebug() << "User rejects ";
}

