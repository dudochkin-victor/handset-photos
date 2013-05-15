/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef GET_ALL_PHOTOS_WORKER_H
#define GET_ALL_PHOTOS_WORKER_H

#include <QThread>
#include <QPixmap>

class GetAllPhotosWorker : public QThread {
public:
    static QVector<QStringList> allPhotos;
    static QHash<QString, QStringList> allPhotosMetaHash;
    inline void waitTillDone() {
       mutex.lock();
       while (!isDone) {
           cond.wait(&mutex);
       }
       mutex.unlock();
    };

    GetAllPhotosWorker() : isDone(false) {};
    void run();

    static void sortByDate(QVector<QStringList> &photos);
    static void sortByDate(QStringList &photoUrns);

//    static QString photoUrnToPath(const QString &photoUrn);

    static QDate getPhotoDate(const QString &photoUrn);
    static QDate getPhotoDate(const QStringList &a);

    static GetAllPhotosWorker *instance();
    void addOnePhoto(const QStringList &photo);
    void removeOnePhoto(const QString photoUrn);
    QStringList getPhotoInfo(const QString &photoUrn);

private:
    static bool compare(const QStringList &a, const QStringList &b);
    static bool compare2(const QString &a, const QString &b);
    static GetAllPhotosWorker *worker_instance;
    static QDate extractLastModifiedDate(const QStringList &a);
    
    void storePhotoMetaInfo(QVector<QStringList> &allPhotos);

    bool isDone;
    QWaitCondition cond;
    QMutex mutex;
    QMutex mutexForAllPhotos;
};

#endif // UTILS_H
