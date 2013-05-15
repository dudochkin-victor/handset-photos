/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */


#ifndef SEARCHENGINE_H
#define SEARCHENGINE_H
#include <QtCore>

class SearchEngine
{
public:
    static bool getAllPhoto(QVector<QStringList> &result);
    static inline QString getPhotoUrn(const QStringList &photoResult)
    {
        return photoResult[2];
    }

    static inline QString getPhotoUri(const QStringList &photoResult)
    {
        return photoResult[0];
    }

    static inline QString getPhotoLastModified(const QStringList &photoResult)
    {
        return photoResult[3];
    }

    static inline QString getPhotoMimeType(const QStringList &photoResult)
    {
        return photoResult[1];
    }

    static inline QString getPhotoContentCreated(const QStringList &photoResult)
    {
        return photoResult[4];
    }

    /// @brief get all photo albums from tracker, including nie:title; nie:identifier; nfo:entryCounter
    static bool getAllPhotoAlbums(QVector<QStringList> &result);

    /// @brief get all photos from specific album, including photo urls, nie:contentCreated
    static bool getAlbumPhotos(const QString &albumIdentifier, QVector<QStringList> &result);

    /// @brief create an album, photoFiles must contain urn:uuid:xxxxxx
    static bool createAlbum(const QString &albumIdentifier, const QString &albumTitle, const QStringList &photoFiles);

    /// @brief delete an album
    static bool deleteAlbum(const QString &albumIdentifier);

    /// @brief set cover for an album
    static bool setAlbumCover(const QString &albumURN, const QString &coverPath);

    /// @brief get cover for an album
    static bool getAlbumCover(const QString &albumURN, QString &coverPath);

    /// @brief get album URN from identifier
//    static bool getAlbumURN(const QString &albumIdentifier, QString &albumURN);

    /// @brief set Tag for an object
    static bool setObjectTag(const QString &objectURN, const QString &tagIdentifier, const QString &tagInfo);

    /// @brief get Tag for an object
    static bool getObjectTag(const QString &objectURN, const QString &tagIdentifier, QString &tagInfo);

    /// @brief delete Tag for an object
    static bool deleteObjectTag(const QString &objectURN, const QString &tagIdentifier);

    /// @brief get url from urn
    static QString getUrl(const QString &objectURN);

    static QString getMimeType(const QString &objectURN);

    static QStringList getOnePhoto(const QString &photoUrn);

};
#endif
