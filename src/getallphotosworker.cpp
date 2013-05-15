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
#include "getallphotosworker.h"
#include "mpsettings.h"
#include "exifutils.h"

GetAllPhotosWorker *GetAllPhotosWorker::worker_instance = NULL;
QVector<QStringList> GetAllPhotosWorker::allPhotos;
QHash<QString, QStringList> GetAllPhotosWorker::allPhotosMetaHash;

void GetAllPhotosWorker::storePhotoMetaInfo(QVector<QStringList> &allPhotos) 
{
    for (int i = 0; i < allPhotos.count(); i++) {
        QString photoUrn = SearchEngine::getPhotoUrn(allPhotos[i]);
        allPhotosMetaHash.insert(photoUrn, allPhotos[i]);
    }
}

void GetAllPhotosWorker::addOnePhoto(const QStringList &photo)
{
    mutexForAllPhotos.lock();
    allPhotos << photo;
    allPhotosMetaHash.insert(SearchEngine::getPhotoUrn(photo), photo);
    sortByDate(allPhotos);
    mutexForAllPhotos.unlock();
}

QStringList GetAllPhotosWorker::getPhotoInfo(const QString &photoUrn)
{
    mutexForAllPhotos.lock();
    QStringList ret = allPhotosMetaHash.value(photoUrn);
    mutexForAllPhotos.unlock();
    return ret;
}

void GetAllPhotosWorker::removeOnePhoto(const QString photoUrn)
{
    mutexForAllPhotos.lock();
    QStringList photoInfo = allPhotosMetaHash.value(photoUrn);
    if (photoInfo.count() > 0) {
        allPhotos.remove(allPhotos.indexOf(photoInfo));
        allPhotosMetaHash.remove(photoUrn);
    }
    mutexForAllPhotos.unlock();
}

void GetAllPhotosWorker::run()
{
    QTime t2;
    t2.start();
    SearchEngine::getAllPhoto(allPhotos);
    qDebug() << "12345: getAllPhotos elapsed:" << t2.elapsed();
    QTime t;
    t.start();
    storePhotoMetaInfo(allPhotos);
    qDebug() << "12345: storePhotoMetaInfo elapsed:" << t.elapsed();
    t.start();
    sortByDate(allPhotos);
    qDebug() << "12345: sortByDate all photos t:" << t.elapsed();
    mutex.lock();
    isDone = true;
    mutex.unlock();
    cond.wakeAll();
}

QDate GetAllPhotosWorker::getPhotoDate(const QString &photoUrn)
{
    QStringList photoMeta = allPhotosMetaHash.value(photoUrn);
    qDebug() << "getPhotoDate, meta:" << photoMeta.count();
    return getPhotoDate(photoMeta);
}

QDate GetAllPhotosWorker::extractLastModifiedDate(const QStringList &a)
{
    return QDate::fromString(((a[3].split("T"))[0]), "yyyy-MM-dd");
}

QDate GetAllPhotosWorker::getPhotoDate(const QStringList &a)
{
    QString dateString = SearchEngine::getPhotoContentCreated(a);
    if (dateString.isEmpty())
        dateString = SearchEngine::getPhotoLastModified(a);

    return QDate::fromString(((dateString.split("T"))[0]), "yyyy-MM-dd");
    /*(
    QDate date;
    QString uri = a[0];
    QString path = QUrl(uri).toLocalFile();
    bool result = ExifUtils(path).getDate(date);
    if (!result) {
        date = extractLastModifiedDate(a);
    }
    return date;
    */
}

bool GetAllPhotosWorker::compare(const QStringList &a, const QStringList &b)
{
    QDate dateOnlyA = getPhotoDate(a);
    QDate dateOnlyB = getPhotoDate(b);
    return dateOnlyA > dateOnlyB;
}

bool GetAllPhotosWorker::compare2(const QString &a, const QString &b)
{
    QTime t;
    static int i = 0;
    t.start();
    bool res = getPhotoDate(a) > getPhotoDate(b);
    qDebug() << "12345 compare2 no: " << i++ << " time; " << t.elapsed();
    return res;
}

void GetAllPhotosWorker::sortByDate(QStringList &photoUrns)
{
    qSort(photoUrns.begin(), photoUrns.end(), GetAllPhotosWorker::compare2);
}

void GetAllPhotosWorker::sortByDate(QVector<QStringList> &photos)
{
    qSort(photos.begin(), photos.end(), GetAllPhotosWorker::compare);
}

GetAllPhotosWorker *GetAllPhotosWorker::instance()
{
    /* no need lock. This will be called at the very beginning */
    if (!worker_instance)
        worker_instance = new GetAllPhotosWorker();
    return worker_instance;
}
