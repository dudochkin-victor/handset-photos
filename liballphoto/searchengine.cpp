/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */


#include "searchengine.h"
#include <QtTracker/Tracker>
static const QString SqlGetAllPhoto = "SELECT nie:url(nie:isStoredAs(?photo)) nie:mimeType(?photo) ?photo nfo:fileLastModified(?photo) nie:contentCreated(?photo) WHERE{?photo a nmm:Photo}";

static const QString SqlGetAllPhotoAlbums = "SELECT nie:identifier(?imagelist) nie:title(?imagelist) nfo:entryCounter(?imagelist) ?imagelist " \
                                             " nao:identifier(?tag) nao:prefLabel(?tag) ?tag  " \
                                             " WHERE {?imagelist a nmm:ImageList . OPTIONAL {?imagelist nao:hasTag ?tag . ?tag nao:identifier 'AlbumCover' . }}";

static const QString SqlGetAlbumPhotos = "SELECT nie:url(nie:isStoredAs(?image)) nie:contentCreated(?image) ?image nie:identifier(?imagelist) nie:title(?imagelist) nfo:entryCounter(?imagelist) "\
                                          " WHERE { ?imagelist nfo:hasMediaFileListEntry ?entry . ?entry nfo:entryContent ?image "\
                                                    " { SELECT ?imagelist WHERE {?imagelist a nmm:ImageList ; nie:identifier '%1'} } }";

static const QString SqlRemoveAlbum = " DELETE { ?photoalbum a nmm:ImageList } WHERE { ?photoalbum nie:identifier '%1' }";

static const QString SqlCreateAlbumBegin = "INSERT { _:a a nmm:ImageList ; nie:identifier '%1' ; nie:title '%2' ; nfo:entryCounter %3 ";
static const QString SqlInsertItem = " nfo:hasMediaFileListEntry [ a nfo:MediaFileListEntry; nfo:entryContent <%1>; nfo:listPosition %2 ] ";
static const QString SqlCreateAlbumEnd = " }";

static const QString SqlQueryAlbumCoverTag = "SELECT ?tag nao:prefLabel(?tag) " \
                                         " WHERE {?imagelist a nmm:ImageList; nao:hasTag ?tag . ?tag nao:identifier 'AlbumCover' . FILTER (str(?imagelist) = '%1')}";
static const QString SqlDeleteAlbumCoverTag = "DELETE {?tag a rdfs:Resource } WHERE {?imagelist nao:hasTag ?tag . ?tag nao:identifier 'AlbumCover' . " \
                                           "{ SELECT ?imagelist WHERE {?imagelist a nmm:ImageList . FILTER (str(?imagelist) = '%1') } } }";
static const QString SqlInsertAlbumCoverTag = "INSERT { _:tag a nao:Tag ; nao:prefLabel '%1' ; nao:identifier 'AlbumCover' . ?imagelist nao:hasTag _:tag } " \
                                        " WHERE { ?imagelist a nmm:ImageList . FILTER (str(?imagelist) = '%2') }";
static const QString SqlGetAlbumURN = "SELECT ?imagelist WHERE {?imagelist a nmm:ImageList ; nie:identifier '%1' }";

static const QString SqlQueryObjectTag = "SELECT nao:prefLabel(?tag) nao:identifier(?tag) ?tag " \
                                         " WHERE {?object a nie:InformationElement; nao:hasTag ?tag . ?tag nao:identifier '%1' . " \
                                         " FILTER (str(?object) = '%2')}";
static const QString SqlInsertObjectTag = "INSERT { _:tag a nao:Tag ; nao:prefLabel '%1' ; nao:identifier '%2' . ?object nao:hasTag _:tag } " \
                                          " WHERE { ?object a nie:InformationElement . FILTER (str(?object) = '%3') }";
static const QString SqlDeleteObjectTag = "DELETE {?tag a rdfs:Resource } WHERE {?object nao:hasTag ?tag . ?tag nao:identifier '%1' . " \
                                          " { SELECT ?object WHERE {?object a nie:InformationElement . FILTER (str(?object) = '%2') } } }";

static const QString SqlGetUrlFromURN = "SELECT ?url {<%1> nie:url ?url}";
static const QString SqlGetMimeTypeFromURN = "SELECT ?mimetype {<%1> nie:mimeType ?mimetype}";

bool SearchEngine::getAllPhoto(QVector<QStringList> &result)
{
    result = ::tracker()->rawSparqlQuery(SqlGetAllPhoto);
    return TRUE;
}

bool SearchEngine::getAllPhotoAlbums(QVector<QStringList> &result)
{
    result = ::tracker()->rawSparqlQuery(SqlGetAllPhotoAlbums);
    return TRUE;
}

bool SearchEngine::getAlbumPhotos(const QString &albumIdentifier, QVector<QStringList> &result)
{
    QString sql = QString(SqlGetAlbumPhotos).arg(albumIdentifier);
    result = ::tracker()->rawSparqlQuery(sql);
    return TRUE;
}

bool SearchEngine::createAlbum(const QString &albumIdentifier, const QString &albumTitle, const QStringList &photoFiles)
{
    if (photoFiles.size() < 1)
        return TRUE;

    QString sql = QString(SqlCreateAlbumBegin).arg(albumIdentifier).arg(albumTitle).arg(photoFiles.size());

    int j = 0;
    for (QStringList::const_iterator i = photoFiles.begin(); i != photoFiles.end(); i++) {
        QString sqlItem = QString(SqlInsertItem).arg(*i).arg(j++);
        sql += QString(";") + sqlItem;
    }
    sql += SqlCreateAlbumEnd;

    ::tracker()->rawSparqlUpdateQuery(sql);
    return TRUE;
}

bool SearchEngine::deleteAlbum(const QString &albumIdentifier)
{
    QString sql = QString(SqlRemoveAlbum).arg(albumIdentifier);
    ::tracker()->rawSparqlUpdateQuery(sql);
    return TRUE;
}

bool SearchEngine::setAlbumCover(const QString &albumURN, const QString &coverPath)
{
    QString sql = QString(SqlQueryAlbumCoverTag).arg(albumURN);
    bool needDelete = FALSE;

    QVector<QStringList> result = ::tracker()->rawSparqlQuery(sql);

    for (QVector<QStringList>::iterator i = result.begin(); i != result.end(); i++) {
        if (!(*i)[1].isEmpty()) {
            needDelete = TRUE;
            break;
        }
    }

    if (needDelete) {
        sql = QString(SqlDeleteAlbumCoverTag).arg(albumURN);
        ::tracker()->rawSparqlUpdateQuery(sql);
    }

    sql = QString(SqlInsertAlbumCoverTag).arg(coverPath).arg(albumURN);
    ::tracker()->rawSparqlUpdateQuery(sql);

    return TRUE;
}

bool SearchEngine::getAlbumCover(const QString &albumURN, QString &coverPath)
{
    QString sql = QString(SqlQueryAlbumCoverTag).arg(albumURN);

    QVector<QStringList> result = ::tracker()->rawSparqlQuery(sql);

    coverPath = QString("");

    for (QVector<QStringList>::iterator i = result.begin(); i != result.end(); i++) {
        if (!(*i)[1].isEmpty()) {
            coverPath = (*i)[1];
            break;
        }
    }

    return TRUE;
}
/*
bool SearchEngine::getAlbumURN(const QString &albumIdentifier, QString &albumURN)
{
    QString sql = QString(SqlGetAlbumURN).arg(albumIdentifier);

    qDebug() << "SearchEngine::getAlbumURN " << sql;

    QVector<QStringList> result = ::tracker()->rawSparqlQuery(sql);
    if (result.count() > 0)
        albumURN = result[0][0];

    return TRUE;
}
*/
bool SearchEngine::setObjectTag(const QString &objectURN, const QString &tagIdentifier, const QString &tagInfo)
{
    QString sql = QString(SqlInsertObjectTag).arg(tagInfo).arg(tagIdentifier).arg(objectURN);


    ::tracker()->rawSparqlUpdateQuery(sql);

    return TRUE;
}

bool SearchEngine::getObjectTag(const QString &objectURN, const QString &tagIdentifier, QString &tagInfo)
{
    QString sql = QString(SqlQueryObjectTag).arg(tagIdentifier).arg(objectURN);


    QVector<QStringList> result = ::tracker()->rawSparqlQuery(sql);

    tagInfo = "";

    for (QVector<QStringList>::iterator i = result.begin(); i != result.end(); i++) {
        if (!(*i)[0].isEmpty()) {
            tagInfo = (*i)[0];
            break;
        }
    }

    return TRUE;
}

bool SearchEngine::deleteObjectTag(const QString &objectURN, const QString &tagIdentifier)
{
    QString sql = QString(SqlDeleteObjectTag).arg(tagIdentifier).arg(objectURN);


    ::tracker()->rawSparqlUpdateQuery(sql);

    return TRUE;
}

QString SearchEngine::getUrl(const QString &objectURN)
{
    QString sql = QString(SqlGetUrlFromURN).arg(objectURN);
    QString url;


    QVector<QStringList> result = ::tracker()->rawSparqlQuery(sql);
    if (result.count() != 0)
        url = result[0][0];
    return url;
}

QString SearchEngine::getMimeType(const QString &objectURN)
{
    QString sql = QString(SqlGetMimeTypeFromURN).arg(objectURN);
    QString mimeType;


    QVector<QStringList> result = ::tracker()->rawSparqlQuery(sql);
    mimeType = result[0][0];
    return mimeType;
}
