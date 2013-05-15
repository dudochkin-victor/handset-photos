/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include <QString>
#include <QStringList>
#include <QDate>
#include <QDebug>

#include "mpsettings.h"


const QString MPSettings::PHOTO_LOCATION_KEY = "MP_PHOTO_LOCATION_KEY";
const QString MPSettings::PHOTO_ALBUM_URN_KEY = "MP_PHOTO_ALBUM_URN_KEY";
const QString MPSettings::PHOTO_DATE_KEY = "MP_PHOTO_DATE_KEY";
const QString MPSettings::PHOTO_LAST_MODIFIED_DATE_KEY = "MP_PHOTO_LAST_MODIFIED_DATE_KEY";
const QString MPSettings::PHOTO_LOCAL_PATH_KEY = "MP_PHOTO_LOCAL_PATH_KEY";
const QString MPSettings::PHOTO_URI_KEY = "MP_PHOTO_URI_KEY";

const QString MPSettings::ALBUM_FROM_DATE_KEY = "MP_ALBUM_FROM_DATE_KEY";
const QString MPSettings::ALBUM_TO_DATE_KEY = "MP_ALBUM_TO_DATE_KEY";
const QString MPSettings::ALBUM_LOCATION_KEY = "MP_ALBUM_LOCATION_KEY";
const QString MPSettings::ALBUM_NAME_KEY = "MP_ALBUM_NAME_KEY";
const QString MPSettings::ALBUM_ID_KEY = "MP_ALBUM_ID_KEY";
const QString MPSettings::ALBUM_COVER_KEY = "MP_ALBUM_COVER_KEY";
const QString MPSettings::ALBUM_PHOTONUM_KEY = "MP_ALBUM_PHOTONUM_KEY";


MPSettings::MPSettings(const QString & organization, const QString & application, QObject * parent) : 
    settings(organization, application, parent)
{
}

MPSettings::MPSettings()
{
}

MPSettings::~MPSettings()
{
}

void MPSettings::setValue(const QString &photoUrn, const QString &key, const QVariant &value)
{
    MetaData photoMeta;
    photoMeta = settings.value(photoUrn, photoMeta).value<MetaData>();
        
    photoMeta.insert(key, value);
    settings.setValue(photoUrn, photoMeta);
}

QVariant MPSettings::value(const QString &photoUrn, const QString &key) const
{
    MetaData photoMeta;
    photoMeta = settings.value(photoUrn).value<MetaData>();
    return photoMeta.value(key);
}

void MPSettings::remove(const QString &urn)
{
    /* If key is emtpy, all keys in the same group() will be removed 
     * We don't allow this happen */
    if (!urn.isEmpty())
        settings.remove(urn);
}

void MPSettings::setPhotoLocation(const QString &photoUrn, const QString &location)
{
    setValue(photoUrn, PHOTO_LOCATION_KEY, location);
}

QString MPSettings::getPhotoLocation(const QString &photoUrn)
{
    return value(photoUrn, PHOTO_LOCATION_KEY).toString();
}

#if 0
void MPSettings::setPhotoUri(const QString &photoUrn, const QString &uri)
{
    setValue(photoUrn, PHOTO_URI_KEY, uri);
}

QString MPSettings::getPhotoUri(const QString &photoUrn)
{
    return value(photoUrn, PHOTO_URI_KEY).toString();
}
#endif
       
void MPSettings::setPhotoAlbumUrn(const QString &photoUrn, const QString &albumUrn)
{
    setValue(photoUrn, PHOTO_ALBUM_URN_KEY, albumUrn);
}

QString MPSettings::getPhotoAlbumUrn(const QString &photoUrn)
{
    return value(photoUrn, PHOTO_ALBUM_URN_KEY).toString();
}

#if 0
void MPSettings::setPhotoLocalPath(const QString &photoUrn, const QString &localPath)
{
    setValue(photoUrn, PHOTO_LOCAL_PATH_KEY, localPath);
}

QString MPSettings::getPhotoLocalPath(const QString &photoUrn)
{
    return value(photoUrn, PHOTO_LOCAL_PATH_KEY).toString();
}
#endif     
/*
void MPSettings::setPhotoAlbumName(const QString &photoUrn, const QString &albumName)
{
    setValue(photoUrn, ALBUM_NAME_KEY, albumName);
}
*/

QString MPSettings::getPhotoAlbumName(const QString &photoUrn)
{
    QString albumUrn = value(photoUrn, PHOTO_ALBUM_URN_KEY).toString();
    return getAlbumName(albumUrn);
}

#if 0
void MPSettings::setPhotoDate(const QString &photoUrn, const QString &dateString)
{
    setValue(photoUrn, PHOTO_DATE_KEY, dateString);
}

QString MPSettings::getPhotoDate(const QString &photoUrn)
{
    return value(photoUrn, PHOTO_DATE_KEY).toString();
}

void MPSettings::setPhotoLastModifiedDate(const QString &photoUrn, const QString &dateString)
{
    setValue(photoUrn, PHOTO_LAST_MODIFIED_DATE_KEY, dateString);
}

QString MPSettings::getPhotoLastModifiedDate(const QString &photoUrn)
{
    return value(photoUrn, PHOTO_LAST_MODIFIED_DATE_KEY).toString();
}
#endif

void MPSettings::setAlbumDate(const QString &albumUrn, const QString &fromDateString, const QString &toDateString)
{
    MetaData &albumInfo = albumMeta[albumUrn];
        
    albumInfo.insert(ALBUM_FROM_DATE_KEY, fromDateString);
    albumInfo.insert(ALBUM_TO_DATE_KEY, toDateString);

    setValue(albumUrn, ALBUM_FROM_DATE_KEY, fromDateString);
    setValue(albumUrn, ALBUM_TO_DATE_KEY, toDateString);
}

void MPSettings::getAlbumDate(const QString &albumUrn, QString &fromDateString, QString &toDateString)
{
    fromDateString = value(albumUrn, ALBUM_FROM_DATE_KEY).toString();
    toDateString = value(albumUrn, ALBUM_TO_DATE_KEY).toString();
    qDebug() << "paul debug album date get album " << albumUrn << "date " << fromDateString;
}

QString MPSettings::getAlbumLocation(const QString &albumUrn)
{
    return value(albumUrn, ALBUM_LOCATION_KEY).toString();
}

void MPSettings::setAlbumLocation(const QString &albumUrn, const QString &location)
{
    MetaData &albumInfo = albumMeta[albumUrn];
        
    albumInfo.insert(ALBUM_LOCATION_KEY, location);

    setValue(albumUrn, ALBUM_LOCATION_KEY, location);
}

void MPSettings::setAlbumName(const QString &albumUrn, const QString &name)
{/*
    MetaData &albumInfo = albumMeta[albumUrn];
        
    albumInfo.insert(ALBUM_NAME_KEY, name);
    setValue(albumUrn, ALBUM_NAME_KEY, name); */
}

QString MPSettings::getAlbumName(const QString &albumUrn)
{
    QString start, end;
    QString location;
    QString name;
    location = getAlbumLocation(albumUrn);
    if (!location.isEmpty()) {
        name = location + " "; 
    } 
    getAlbumDate(albumUrn, start, end);
    QDate startDate = QDate::fromString(start);
    QDate today = QDate::currentDate();
    int delta = startDate.daysTo(today);
    if (delta == 0)
        start = "Today";
    else if (delta == 1)
        start = "Yesterday";
    else if (delta == -1)
        start = "Tomorrow"; //possible????

    name.append(start);
    return name;
}

void MPSettings::setAlbumID(const QString &albumUrn, const QString &name)
{
}

QString MPSettings::getAlbumID(const QString &albumUrn)
{
    return albumUrn;
}

void MPSettings::setAlbumCover(const QString &albumUrn, const QString &coverPhotoURI)
{
    MetaData &albumInfo = albumMeta[albumUrn];
        
    albumInfo.insert(ALBUM_COVER_KEY, coverPhotoURI);
    setValue(albumUrn, ALBUM_COVER_KEY, coverPhotoURI);
}

QString MPSettings::getAlbumCover(const QString &albumUrn)
{
    return value(albumUrn, ALBUM_COVER_KEY).toString();
}

void MPSettings::getAllAlbums(QVector<QStringList> &result)
{
    QList<QString> keys = albumMeta.keys();
    result.resize(keys.count());
    for (int i = 0; i < keys.count(); i++) {
        QStringList t;
        t << keys[i];   //t[0] is urn
        t << "";    //t[1] is albumName which is not used anymore
        MetaData &albumInfo =  albumMeta[keys[i]];
        t << QString("%1").arg(albumInfo[ALBUM_PHOTONUM_KEY].toInt());
        result[i] = t;
    }
}
    
void MPSettings::setAlbumPhotoNum(const QString &albumID, const int num)
{
    MetaData &albumInfo = albumMeta[albumID];
        
    albumInfo.insert(ALBUM_PHOTONUM_KEY, num);
}

int MPSettings::getAlbumPhotoNum(const QString &albumID)
{
    MetaData &albumInfo = albumMeta[albumID];
        
    return albumInfo[ALBUM_PHOTONUM_KEY].toInt();
}

void MPSettings::removeAlbum(const QString &albumID)
{
    albumMeta.remove(albumID);
}

