/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include <QtTracker/Tracker>
#include <QDate>
#include <QFileInfo>
#include <QUrl>
#include <QtAlgorithms>
#include <QUuid>

#include "searchengine.h"
#include "albumsmerger.h"
#include "../appwindow.h"
#include "mpsettings.h"
#include "../getallphotosworker.h"
#include "yahooreversegeocoder.h"
#include "exifutils.h"
#include "../trackerlistener.h"


AlbumsMerger::AlbumsMerger()
{
    geocoder = new YahooReverseGeocoder(this);
    connect(TrackerListener::instance(), SIGNAL(SubjectsAdded(const QStringList &)), this, SLOT(handlePhotoAdded(const QStringList &)), Qt::QueuedConnection);
    connect(TrackerListener::instance(), SIGNAL(SubjectsRemoved(const QStringList &)), this, SLOT(handlePhotoRemoved(const QStringList &)), Qt::QueuedConnection);
}

AlbumsMerger::~AlbumsMerger()
{
    delete geocoder;
}

QString AlbumsMerger::createDateAlbumID(const DateRange &dateRange, int subid)
{
    return QString("dateAlbum,") + dateRange.toString() + (subid > 0 ? QString(",%1").arg(subid) : ",");// + QUuid::createUuid() ;
}

DateRange AlbumsMerger::dateRangeFromAlbumID(const QString &dateAlbumID)
{
    QStringList list;
    list = dateAlbumID.split(",");
    QString dateRangeString;
    if (list.count() > 1) {
        dateRangeString = list[1];
    }
    return DateRange::fromString(dateRangeString);
}

void AlbumsMerger::createAlbumHelper(const DateRange &dateRange, const QStringList &photos, int subid)
{ 
    if (photos.count() == 0)
        return;

    QString albumID = createDateAlbumID(dateRange, subid);
    //SearchEngine::createAlbum(albumID, albumID, photos);
    trackerTasks << TrackerTask(albumID, TrackerTask::CreateAlbum, photos);
    QString albumUrn = albumID;
//    settings.setAlbumName(albumUrn, albumName);
    for(int i = 0; i < photos.size(); i++) {
        AppWindow::instance()->settings.setPhotoAlbumUrn(photos.at(i), albumUrn);
    }

    QStringList photoInfo = GetAllPhotosWorker::instance()->getPhotoInfo(photos[0]);
    QString urlString = SearchEngine::getPhotoUri(photoInfo);
    AppWindow::instance()->settings.setAlbumCover(albumUrn, urlString);
    AppWindow::instance()->settings.setAlbumPhotoNum(albumUrn, photos.count());
    AppWindow::instance()->settings.setAlbumDate(albumUrn, dateRange.getStartDate().toString(), dateRange.getEndDate().toString());
    AppWindow::instance()->settings.setAlbumID(albumUrn, albumID);
} 

void AlbumsMerger::deleteOneAlbum(const QString &id)
{
    QString albumID = id;
    QString albumUrn = albumID;
    //SearchEngine::deleteAlbum(albumID);
    trackerTasks << TrackerTask(albumID, TrackerTask::DeleteAlbum);
    AppWindow::instance()->settings.remove(albumUrn);
    AppWindow::instance()->settings.removeAlbum(albumUrn);
}

void AlbumsMerger::deleteAllAlbums()
{
    QVector<QStringList> albums;
    bool result = SearchEngine::getAllPhotoAlbums(albums);
    if (!result)
        return;
    

    for (int i = 0; i < albums.count(); i++) {
        deleteOneAlbum(albums[i][0]);
    }
}

DateRange &AlbumsMerger::getDateRange(const QString &photoUrn)
{
    QDate date = GetAllPhotosWorker::getPhotoDate(photoUrn);
    DateRange range(date, date);
    int index = dateRanges.indexOf(range);
    if (index == -1) {
        dateRanges << range;
        ::qSort(dateRanges.begin(), dateRanges.end(), qGreater<DateRange>());
        index = dateRanges.indexOf(range);
    }

    return dateRanges[index];
}

uint qHash(const DateRange &key)
{
    return qHash(key.getStartDate().toJulianDay() + key.getEndDate().toJulianDay());
}

bool AlbumsMerger::mergeOnly(QVector<QStringList> *photos)
{
    QVector<QStringList> dateOnlyPhotos;
    QVector<QStringList> locationPhotos;
    dividePhotos(*photos, dateOnlyPhotos, locationPhotos);
    dateOnlyMerge(&dateOnlyPhotos);
    locationOnlyMerge(&locationPhotos);
    emit albumsChanged();
    QTimer::singleShot(0, this, SLOT(doTrackerTasks()));
    return true;
}
 
bool AlbumsMerger::merge(QVector<QStringList> *photos)
{
    QTime t;
    t.start();
    deleteAllAlbums();
    return mergeOnly(photos);
}

struct AlbumMeta {
    AlbumMeta(const QStringList &photos, const DateRange &range, int index) : photos(photos), range(range), index(index) {};
    QStringList photos; //photos belong to this album
    DateRange range;
    int index;  //If this album is a splited one, this is the index among all splited albums
};


bool AlbumsMerger::dateOnlyMerge(QVector<QStringList> *photos)
{
    if (photos->count() <= 0)
        return false;

    QHash<QString, AlbumMeta> albumIDMetaHash;
    for (QVector<QStringList>::iterator i = photos->begin(); i != photos->end(); i++) {
        QString urn = (*i)[2];
        QTime t;
        t.start();
        DateRange &dateRange = getDateRange(urn);

        if (!dateAlbumsHash[dateRange].contains(urn)) {
            t.start();
            dateAlbumsHash[dateRange] << urn;
            QStringList &photoInAlbums = dateAlbumsHash[dateRange];
    
            QVector<QStringList> photosVec;
            QVector<QString> dateRangesVec;
            int numSplitAlbums = photoInAlbums.count() / 250 + 1;
            photosVec.resize(numSplitAlbums);
          //  dateRangesVec.resize(numSplitAlbums);
            int numPhotos = photoInAlbums.count() / numSplitAlbums;
            for (int j = 0; j < numSplitAlbums; j++) {
                for (int i = j*numPhotos; i < (j+1)*numPhotos && i < photoInAlbums.count(); i++)
                    photosVec[j] << photoInAlbums.at(i);
            //    locationVec[j] = dateRange + (j > 0) ? QString(" %1").arg(j) : "";
            }


            /* Tracker can't support add one photo into existing album
             * workaound here: remove the album and create a new one */
            for (int j = 0; j < numSplitAlbums; j++) {
                QString albumID = createDateAlbumID(dateRange, j);
                albumIDMetaHash.insert(albumID, AlbumMeta(photosVec[j], dateRange, j));
            }
        }
    }

    for (QHash<QString, AlbumMeta>::const_iterator i = albumIDMetaHash.constBegin(); i != albumIDMetaHash.constEnd(); i++) {
        const QString albumID = i.key();
        const AlbumMeta meta = i.value();
        deleteOneAlbum(albumID);
        createAlbumHelper(meta.range, meta.photos, meta.index);
    }

    return true;
}

QString AlbumsMerger::createLocationAlbumID(const QString &location)
{
    return QString("locationAlbum") + location;// + QUuid::createUuid();
}

void AlbumsMerger::addOneLocationPhotoIntoAlbum(const QString &urn, const QString &location)
{
    if (!locations.contains(location))
        locations << location;
    QStringList &photos = locationAlbumsHash[location];
    if (!photos.contains(urn)) {
        photos << urn;
        GetAllPhotosWorker::sortByDate(photos);
        QVector<QStringList> photosVec;
        QVector<QString> locationVec;
        int numSplitAlbums = photos.count() / 250 + 1;
        photosVec.resize(numSplitAlbums);
        locationVec.resize(numSplitAlbums);
        QStringList firstHalf;
        QStringList secondHalf;
        int numPhotos = photos.count() / numSplitAlbums;
        for (int j = 0; j < numSplitAlbums; j++) {
            for (int i = j*numPhotos; i < (j+1)*numPhotos && i < photos.count(); i++)
                photosVec[j] << photos.at(i);
            locationVec[j] = location + ((j > 0) ? QString(" %1").arg(j) : "");
        }

        for (int j = 0; j < numSplitAlbums; j++) {
            QString albumID = createLocationAlbumID(locationVec[j]);
            deleteOneAlbum(albumID);
            //SearchEngine::createAlbum(albumID, albumID, photosVec[j]);
            trackerTasks << TrackerTask(albumID, TrackerTask::CreateAlbum, photosVec[j]);
            QString albumUrn = albumID;
            for(int i = 0; i < photosVec[j].size(); i++) {
                AppWindow::instance()->settings.setPhotoAlbumUrn(photosVec[j].at(i), albumUrn);
            }

            QStringList photoInfo = GetAllPhotosWorker::instance()->getPhotoInfo(photosVec[j][0]);
            QString urlString = SearchEngine::getPhotoUri(photoInfo);

            //        QUrl coverUrl(urlString);

            AppWindow::instance()->settings.setAlbumCover(albumUrn, urlString);
            AppWindow::instance()->settings.setAlbumDate(albumUrn, GetAllPhotosWorker::getPhotoDate(photosVec[j][0]).toString(),
                    GetAllPhotosWorker::getPhotoDate(photosVec[j].last()).toString());
            AppWindow::instance()->settings.setAlbumID(albumUrn, albumID);
            AppWindow::instance()->settings.setAlbumPhotoNum(albumUrn, photosVec[j].count());
            AppWindow::instance()->settings.setAlbumLocation(albumUrn, locationVec[j]);
        }
        emit albumsChanged();
    }
}

bool AlbumsMerger::locationOnlyMerge(QVector<QStringList> *photos)
{
    for (QVector<QStringList>::iterator i = photos->begin(); i != photos->end(); i++) {
        QString urn = SearchEngine::getPhotoUrn(*i);
            QTime t;
            t.start();
        QString location = AppWindow::instance()->settings.getPhotoLocation(urn);

        if (!location.isEmpty()) {
            QTime t;
            t.start();
            addOneLocationPhotoIntoAlbum(urn, location);
        } else  {
            QStringList photoInfo = GetAllPhotosWorker::instance()->getPhotoInfo(urn);
            QString uri = SearchEngine::getPhotoUri(photoInfo);
            QString localPath = QUrl(uri).toLocalFile();
            ExifUtils exif(localPath);
            float lat, lon;
            exif.getLatitude(lat);
            exif.getLongitude(lon);
            geocoder->reverseGeocoding(QVariant(urn), lat, lon);
        }
    }

    return true;
}

bool AlbumsMerger::isLocationPhoto(const QString &photoUrn)
{
    QString location = AppWindow::instance()->settings.getPhotoLocation(photoUrn);
    bool res = false;
    if (location.isEmpty()) {
        QStringList photoInfo = GetAllPhotosWorker::instance()->getPhotoInfo(photoUrn);
        // if not ready, inset meta data
        if(photoInfo.isEmpty()) {
            photoInfo = SearchEngine::getOnePhoto(photoUrn);
            GetAllPhotosWorker::instance()->addOnePhoto(photoInfo);
        }
        QString uri = SearchEngine::getPhotoUri(photoInfo);
        QString localPath = QUrl(uri).toLocalFile();
        ExifUtils exif(localPath);
        float dummy;
        if (exif.getLongitude(dummy) && exif.getLatitude(dummy))
            res = true;
    } else {
        res = true;
    }

    return res;
} 

void AlbumsMerger::dividePhotos(const QVector<QStringList> &photos, QVector<QStringList> &dateOnlyPhotos, QVector<QStringList> &locationPhotos)
{
    for (int i = 0; i < photos.count(); i++) {
        QString photoUrn = photos[i][2];
        if (isLocationPhoto(photoUrn))
            locationPhotos << photos[i];
        else
            dateOnlyPhotos << photos[i];
    }
}

void AlbumsMerger::onTranslated(const QVariant &userdata, const QString &city)
{
    QString urn = userdata.toString();
    QString location = city;
    if (location.isEmpty())
        location = "Unknown";
    AppWindow::instance()->settings.setPhotoLocation(urn, location);

    addOneLocationPhotoIntoAlbum(urn,location);
    QTimer::singleShot(0, this, SLOT(doTrackerTasks()));
}

void AlbumsMerger::handlePhotoAdded(const QStringList &urns)
{
    QVector<QStringList> x(urns.count());
/*    MPSettings settings;
    for (int i = 0; i < urns.count(); i++) {
        x[i] << "" << "" << urns[i];
        settings.setPhotoUri(urns[i], SearchEngine::getUrl(urns[i]));
    } */
    for (int i = 0; i < urns.count(); i++) {
        x[i] << "" << "" << urns[i];
    }
    mergeOnly(&x);
}

void AlbumsMerger::handlePhotoRemoved(const QStringList &urns)
{
    QString location;
    bool shouldUpdate = false;
    for (int i = 0; i < urns.count(); i++) {
        if (!AppWindow::instance()->photoRemoveBlacklist.contains(urns[i])) {
            location = AppWindow::instance()->settings.getPhotoLocation(urns[i]);
            if (!location.isEmpty()) {
                locationAlbumsHash[location].removeAll(urns[i]);
                if (locationAlbumsHash[location].count() == 0) {
                    locations.remove(locations.indexOf(location));
                    locationAlbumsHash.remove(location);
                    deleteOneAlbum(createLocationAlbumID(location));
                }
            } else {
                // the photo belongs to a date only location
                QString dateAlbumID =  AppWindow::instance()->settings.getPhotoAlbumUrn(urns[i]);
                qDebug() << "123456: dateAlbumID:" << dateAlbumID;
                DateRange range = dateRangeFromAlbumID(dateAlbumID);
                qDebug() << "123456: range:" << range.toString();
                if (dateRanges.contains(range)) {
                    dateAlbumsHash[range].removeAll(urns[i]);
                    if (dateAlbumsHash[range].count() == 0) {
                        dateAlbumsHash.remove(range);
                        dateRanges.remove(dateRanges.indexOf(range));
                    }
                    QString albumID = createDateAlbumID(range);
                    deleteOneAlbum(albumID);
                    if (dateAlbumsHash[range].count() > 0) {
                        createAlbumHelper(range, dateAlbumsHash[range]);
                    }
                }

            }
            shouldUpdate = true;
        } else {
        }
        AppWindow::instance()->settings.remove(urns[i]);
    }
    if (shouldUpdate)
        emit albumsChanged();
    QTimer::singleShot(0, this, SLOT(doTrackerTasks()));
}

void AlbumsMerger::doTrackerTasks()
{
    while (!trackerTasks.isEmpty()) {
        TrackerTask task = trackerTasks.takeFirst();
        task.run();
    }
}


TrackerTask::TrackerTask(const QString &albumID, TrackerTask::Command cmd, const QStringList &photos) : albumID(albumID), command(cmd), photos(photos)
{
}

void TrackerTask::run()
{
    switch (command) {
        case TrackerTask::DeleteAlbum:
            SearchEngine::deleteAlbum(albumID);
            break;
        case TrackerTask::CreateAlbum:
            SearchEngine::createAlbum(albumID, albumID, photos); 
            break;
    }
}
