/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef MPSETTINGS_H
#define MPSETTINGS_H

#include <QString>
#include <QSettings>

class MPSettings {
public:
    MPSettings(const QString & organization, const QString & application = QString(), QObject * parent = 0 );
    MPSettings();
    ~MPSettings();

    void remove(const QString &urn);
     
    void setPhotoLocation(const QString &photoUrn, const QString &location);
    QString getPhotoLocation(const QString &photoUrn);

#if 0    
    void setPhotoUri(const QString &photoUrn, const QString &uri);
    QString getPhotoUri(const QString &photoUrn);
#endif

    void setPhotoAlbumUrn(const QString &photoUrn, const QString &albumUrn);
    QString getPhotoAlbumUrn(const QString &photoUrn);
    
    QString getPhotoAlbumName(const QString &photoUrn);
#if 0
    void setPhotoDate(const QString &photoUrn, const QString &dateString);
    QString getPhotoDate(const QString &photoUrn);
    //void setPhotoLocalPath(const QString &photoUrn, const QString &localPath);
    //QString getPhotoLocalPath(const QString &photoUrn);
    
    void setPhotoLastModifiedDate(const QString &photoUrn, const QString &dateString);
    QString getPhotoLastModifiedDate(const QString &photoUrn);
#endif
    
    void setAlbumDate(const QString &albumUrn, const QString &fromDateString, const QString &toDateString);
    void getAlbumDate(const QString &albumUrn, QString &fromDateString, QString &toDateString);

    QString getAlbumLocation(const QString &albumUrn);
    void setAlbumLocation(const QString &albumUrn, const QString &location);
    
    void setAlbumName(const QString &photoUrn, const QString &albumName);
    QString getAlbumName(const QString &photoUrn);
    
    void setAlbumID(const QString &photoUrn, const QString &albumID);
    QString getAlbumID(const QString &photoUrn);
    
    void setAlbumCover(const QString &albumID, const QString &photoURI);
    QString getAlbumCover(const QString &albumID);

    void setAlbumPhotoNum(const QString &albumID, const int num);
    int getAlbumPhotoNum(const QString &albumID);

    void getAllAlbums(QVector<QStringList> &result);
    void removeAlbum(const QString &albumID);

protected:
    static const QString PHOTO_ALBUM_URN_KEY;    /* ID of the album that the photo belongs to */
    static const QString PHOTO_LOCATION_KEY;
    static const QString PHOTO_DATE_KEY;
    static const QString PHOTO_LOCAL_PATH_KEY;
    static const QString PHOTO_LAST_MODIFIED_DATE_KEY;
    static const QString PHOTO_URI_KEY;

    static const QString ALBUM_FROM_DATE_KEY;
    static const QString ALBUM_TO_DATE_KEY;
    static const QString ALBUM_LOCATION_KEY;
    static const QString ALBUM_NAME_KEY;  /* name of the album that the photo  */
    static const QString ALBUM_ID_KEY;  
    static const QString ALBUM_COVER_KEY; 
    static const QString ALBUM_PHOTONUM_KEY;

    void setValue(const QString &photoUrn, const QString &key, const QVariant &value);
    QVariant value(const QString &photoUrn, const QString &key) const;

private:
    QSettings settings;
    typedef QHash<QString, QVariant> MetaData;
    QHash<QString, MetaData> albumMeta;
};

#endif
