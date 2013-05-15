/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef ALBUMSMERGER_H
#define ALBUMSMERGER_H
#endif // ALBUMSMERGE_H

#include <QDate>
#include <QHash>
#include <QVector>
#include <QStringList>

#include "reversegeocoder.h"
#define MaxAlbums 20

class DateRange
{
public:
    DateRange() {};
    DateRange(const QDate &_start, const QDate &_end) : start(_start), end(_end) {};
    inline bool isInRange(const QDate &date)
    {
        return (date >= start && date <= end);
    }

    inline bool operator>(const DateRange &b) const 
    {
        return start > b.start;
    }

    inline bool operator<(const DateRange &b) const
    {
        return start < b.start;
    }

    inline bool operator==(const DateRange &b) const
    {
        return start == b.start;
    }

    inline QDate getStartDate() const { return start;}
    inline QDate getEndDate() const { return end;}

    inline QString toString() const { return start.toString() + "split" + end.toString();};
    static inline DateRange fromString(const QString &str)
    {
        DateRange result;
        QStringList list = str.split("split");
        if (list.count() == 2) {
            QDate start = QDate::fromString(list[0]);
            QDate end = QDate::fromString(list[1]);
            result = DateRange(start, end);
        }
        return result;
    }

    void splitRange(const DateRange &range, int num, QVector<DateRange> &newRanges);

private:
    QDate start;
    QDate end;
};

class TrackerTask
{
public:
    enum Command {
        DeleteAlbum,
        CreateAlbum
    };
    TrackerTask(const QString &albumID, TrackerTask::Command cmd, const QStringList &photos = QStringList());

    void run();
private:
    Command command;
    QString albumID;
    QStringList photos;
};

class AlbumsMerger : public QObject, public ReverseGeocoderAction
{
    Q_OBJECT
public:
    AlbumsMerger();
    ~AlbumsMerger();

    bool merge(QVector<QStringList> *photos);

    bool dateOnlyMerge(QVector<QStringList> *photos);
    bool locationOnlyMerge(QVector<QStringList> *photos);
    
    virtual void onTranslated(const QVariant &userdata, const QString &location);
    void addUrnToBlackList(const QString &urn);
    
    DateRange dateRangeFromAlbumID(const QString &dateAlbumID);

    //static void storeAlbumNameID(QString urn, QString ID, QString name);
    
signals:
    void albumsChanged();

private:
    void deleteOneAlbum(const QString &id);
    void deleteAllAlbums();
    void createAlbumHelper(const DateRange &dateRange, const QStringList &photos, int subid = 0);
    DateRange &getDateRange(const QString &photoUrn);
    void dividePhotos(const QVector<QStringList> &photo, QVector<QStringList> &dateOnlyPhotos, QVector<QStringList> &locationPhotos);
    void addOneLocationPhotoIntoAlbum(const QString &urn, const QString &location);
    bool isLocationPhoto(const QString &photoUrn);
    bool mergeOnly(QVector<QStringList> *photos);
    QString createDateAlbumID(const DateRange &dateRange, int subid = 0);
    QString createLocationAlbumID(const QString &location);

    QVector<DateRange> dateRanges;
    QHash<DateRange, QStringList> dateAlbumsHash;
    QHash<DateRange, int> numSplitDateAlbumsHash;

    QVector<QString> locations;
    QHash<QString, QStringList> locationAlbumsHash;
    QHash<QString, int> numSplitLocationAlbumsHash;

    ReverseGeocoder *geocoder;

    QList<TrackerTask> trackerTasks;


private slots:
    void handlePhotoAdded(const QStringList &);
    void handlePhotoRemoved(const QStringList &);
    void doTrackerTasks();

};

