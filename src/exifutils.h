/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef EXIF_UTILS_H
#define EXIF_UTILS_H

#include <QString>
#include <QDate>
#include <libexif/exif-data.h>

class ExifUtils {
public:
    ExifUtils(const QString &path);
    ~ExifUtils();
    bool readInfo(QString path, ExifIfd ifd, ExifTag tag, char *buf, int len);
    bool getLatitude(float &latitude);
    bool getLongitude(float &longitude);
    bool getDate(QDate &date);

private:
    bool readDegree(ExifTag tag, float &value);
    bool readTag(ExifTag tag, char *buf, int len);
    ExifData *ed;

};

#endif
