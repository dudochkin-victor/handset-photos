/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef ALBUMSMERGE_H
#define ALBUMSMERGE_H
#endif // ALBUMSMERGE_H

#include <QDate>
#define MaxAlbums 20


class albumsMerge{
public:
    static bool Merge(QVector<QStringList> *photos);
    static void storeAlbumNameID(QString urn, QString ID, QString name);
 
private:
	QString thumbnailURI;	   
 };

