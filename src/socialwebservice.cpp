/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include <QtGui>
#include "socialwebservice.h"
#include "dbustypes.h"
#include "flickr_interface.h"

const QString SocialWebService::SERVICE = "com.meego.libsocialweb";
const QString SocialWebService::PATH_HEAD= "/com/meego/libsocialweb/Service/";

SocialWebService::SocialWebService(QObject *parent) : QObject(parent)
{
    checkServices();

}
#if 0
QStringList SocialWebService::getServices()
{
    return m_services;
}
#endif
void SocialWebService::uploadPhoto(const QString &local_filename, const QString &title)
{
    QUrl url(local_filename);
    QString filename = url.path();
    QMap<QString, QString> map;
    if(title != NULL)
	map["title"] = title;
    FlickrStruct data(map);
    qDebug() << "filename:" << filename << "title:" << title;
    interface->UploadPhoto(filename, data);
}
void SocialWebService::uploadPhotos(const QStringList &local_filenames, const QString album_name)
{
    for(int i = 0; i < local_filenames.size(); ++i) {
	uploadPhoto(local_filenames.at(i), NULL);
    }
    //TODO: create flickr sets for this album (libsocialweb need to support first)

}

void SocialWebService::checkServices()
{
    //TODO: check services and account is ready for upload or not
    m_services << "flickr";
    m_services.removeDuplicates();

    //connect interface
    QString path;
    path.append(PATH_HEAD);
    path.append("flickr");
    interface = new ComMeegoLibsocialwebPhotoUploadInterface(
                SERVICE, path, QDBusConnection::sessionBus(), this);
}
