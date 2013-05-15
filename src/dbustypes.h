/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef DBUSTYPES_H
#define DBUSTYPES_H
#include <QtGui>
#include <QtDBus>

struct FlickrStruct : public QMap<QString, QString>
{
    inline FlickrStruct() : QMap<QString, QString>() {}
    inline FlickrStruct(const QMap<QString, QString>& a) : QMap<QString, QString>(a) {}

    inline FlickrStruct& operator=(const QMap<QString, QString>& a)
    {
        *(static_cast<QMap<QString, QString>*>(this)) = a;
        return *this;
    }
};

Q_DECLARE_METATYPE(FlickrStruct);

#endif //DBUSTYPES_H
