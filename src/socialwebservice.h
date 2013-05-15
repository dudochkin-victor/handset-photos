
/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef SOCIALWEBSERVICE_H
#define SOCIALWEBSERVICE_H

#include <QObject>
#include <QStringList>
#include "dbustypes.h"
#include "flickr_interface.h"

class SocialWebService : public QObject
{
    Q_OBJECT

public:
    SocialWebService(QObject *parent = 0);
    //QStringList getServices();
    void uploadPhoto(const QString &local_filename, const QString &title);
    void uploadPhotos(const QStringList &local_filenames, const QString album_name);

private:
    QStringList m_services;
    void checkServices();
    ComMeegoLibsocialwebPhotoUploadInterface *interface;
    const static QString SERVICE;
    const static QString PATH_HEAD;
};

#endif // SOCIALWEBSERVICE_H
