/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */


#ifndef ALBUM_LIST_DELETE_CONFIRM
#define ALBUM_LIST_DELETE_CONFIRM

#include <QModelIndex>
#include "abstractdeleteconfirm.h"
class AlbumListDeleteConfirm : public AbstractDeleteConfirm {
    Q_OBJECT
public:
    void setAlbum(const QString &album, const QModelIndex &index);

public slots:
    virtual void onRejected();
    virtual void onAccepted();

private:
    QString album;
    QModelIndex index;
};

#endif
