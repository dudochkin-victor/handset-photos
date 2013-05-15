/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include <QDebug>
#include <QFile>
#include <QStringList>

#include "exifutils.h"

ExifUtils::ExifUtils(const QString &path)
{
    QByteArray encodeName = QFile::encodeName(path);
	ed = exif_data_new_from_file(encodeName.data());
}

ExifUtils::~ExifUtils()
{
    if (ed)
        exif_data_free(ed);
}

bool ExifUtils::readTag(ExifTag tag, char *buf, int len)
{
    ExifEntry *entry;
    bool result = false;
   	if(ed !=NULL) {
		entry = exif_data_get_entry(ed, tag);
   		if (entry) {
       		exif_entry_get_value(entry, buf, len);
            result = true;
    	}
	}
	return result; 
}

bool ExifUtils::readDegree(ExifTag tag, float &value)
{
    char buf[64];
    bool result = readTag(tag, buf, sizeof(buf));
    if (!result)
        return result;

    QStringList valList = QString(buf).split(",");
    if (valList.count() != 3)
        return false;
    
    value = valList[0].toFloat() + valList[1].toFloat()/60.0 + valList[2].toFloat()/3600.0;
    return true;
}
	
bool ExifUtils::getLatitude(float &latitude)
{
    char buf[32];
	bool result = readTag((ExifTag)EXIF_TAG_GPS_LATITUDE_REF, buf, sizeof(buf));
    bool ret = false;
	if (result) {
        ret = readDegree((ExifTag)EXIF_TAG_GPS_LATITUDE, latitude);
        if (ret) {
            if(QString(buf).contains('W'))
                latitude = -1 * latitude;
        }
    }
    return ret;
}

bool ExifUtils::getLongitude(float &longitude)
{
    char buf[32];
    bool result;
    result = readTag((ExifTag)EXIF_TAG_GPS_LONGITUDE_REF, buf, sizeof(buf));
    bool ret = false;
    if (result) {
        ret = readDegree((ExifTag)EXIF_TAG_GPS_LONGITUDE, longitude);
        if(ret)
            if (QString(buf).contains('S')) 
                longitude = -1 * longitude;
    }

    return ret;
}

bool ExifUtils::getDate(QDate &date)
{
    QTime start = QTime::currentTime();
    char buf[64];
    bool result = readTag(EXIF_TAG_DATE_TIME, buf, sizeof(buf));
    if (result) {
        QString t(buf);
        date = QDate::fromString(t.split(" ")[0], "yyyy:MM:dd");
    }
    int delta = QTime::currentTime().msecsTo(start);
    qDebug() << "yyyyy:delta is" << delta << " date is " << date;

    return result;
}
