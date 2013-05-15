/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include <QDate>
#include "mpproxylistmodel.h"        
#include "mplistmodel.h"
#include "mpabstractlistmodelops.h"
#include "mpsettings.h"
#include "searchengine.h"
#include "getallphotosworker.h"
#include "appwindow.h"

void MPProxyListModel::parseDate(QString in, int &year, int &month, int &day, int &dayOfAWeek) const
{
    int indexY = in.indexOf(QRegExp("[0-9][0-9][0-9][0-9]"), 0);
    QString y;
    year = month = day = dayOfAWeek = -1;
    if (indexY >= 0) {
        y = in.mid(indexY, 4);
        in.remove(QRegExp("[0-9][0-9][0-9][0-9]"));
        year = y.toInt();
    }

    for (int i = 1; i < 13; i++) {
        QString mName = QDate::shortMonthName(i);
        if (in.contains(mName, Qt::CaseInsensitive)) {
            month = i;
            break;
        } else {
            mName = QDate::longMonthName(i);
            if (in.contains(mName, Qt::CaseInsensitive)) {
                month = i;
                break;
            }
        }
    }

    if (month == -1) {
        int indexM = in.indexOf(QRegExp("[0-9][0-9]"), 0);
        QString m;
        if (indexM >= 0) {
            m = in.mid(indexM, 2);
            in.remove(indexM, 2);
            month = m.toInt();
        }
    }

    qDebug() << "yyyyy:"<< in;
    if (month == -1) {
        int indexM = in.indexOf(QRegExp("[0-9]"), 0);
        QString m;
        if (indexM >= 0) {
            m = in.mid(indexM, 1);
            in.remove(indexM, 1);
            month = m.toInt();
        }
    }

    int indexD = in.indexOf(QRegExp("[0-9][0-9]"), 0);
    QString d;
    if (indexD >= 0) {
        d = in.mid(indexD, 2);
        in.remove(indexD, 2);
        day = d.toInt();
    }

    if (day == -1) {
        int indexD = in.indexOf(QRegExp("[0-9]"), 0);
        if (indexD >= 0) {
            d = in.mid(indexD, 1);
            in.remove(indexD, 1);
            day = d.toInt();
        }
    }
    dayOfAWeek = -1;
    if (day == -1) {
        for (int i = 1; i < 8; i++) {
            QString dName = QDate::shortDayName(i);
            if (in.contains(dName, Qt::CaseInsensitive)) {
                dayOfAWeek = i;
                break;
            } else {
                dName = QDate::longDayName(i);
                if (in.contains(dName, Qt::CaseInsensitive)) {
                    dayOfAWeek = i;
                    break;
                }
            }
        }
    }
}

bool MPProxyListModel::compareDate(const QString &dateString1, const QString &userDateString) const
{
    //Todo: implement this
    //only suport YYYY-MM-DD format by now
    QDate photoDate = QDate::fromString(dateString1);

    int year, month, day, dayOfAWeek;
    parseDate(userDateString, year, month, day, dayOfAWeek);
    bool result = false;
    qDebug() << "photoDate: " << dateString1;
    qDebug() << "user input Date year:" << year << "month:" << month << "day:" << day << "dayOfAWeek:" << dayOfAWeek;


    if (day != -1) {
        if (photoDate.day() == day)
            result = true;
    } else {
        if (dayOfAWeek != -1) {
            if (dayOfAWeek == photoDate.dayOfWeek())
               result = true; 
        } else {
            if (month != -1) {
                if (photoDate.month() == month)
                    result = true;
            } else {
                if (year != -1)
                    if (photoDate.year() == year)
                        result = true;
            }

        }
    }

   return result;
}

bool MPProxyListModel::compareLocation(const QString &locationString1, const QString &userLocationString) const
{
    qDebug() << "paul debug compare location" << "str1" << locationString1 << "str2" << userLocationString;
    if (locationString1.isEmpty() || userLocationString.isEmpty())
        return false;
    if (locationString1.contains(userLocationString, Qt::CaseInsensitive) ||
            userLocationString.contains(locationString1, Qt::CaseInsensitive)) {
        return true;
    }
    return false;
}

bool MPProxyListModel::matchPhoto(const QString &photoUrn, const QString &userInput) const
{
    QString dateString = GetAllPhotosWorker::instance()->getPhotoDate(photoUrn).toString();
    if (compareDate(dateString, userInput)) {
        qDebug() << "MPProxyListModel::filterAcceptsRow compare date returns ture";
        return true;
    }

    //Now location
    QString location = AppWindow::instance()->settings.getPhotoLocation(photoUrn);
    if (compareLocation(location, userInput)) {
        qDebug() << "MPProxyListModel::filterAcceptsRow compare location returns ture";
        return true;
    }

    //Then tags
    //
    return false;
}

bool MPProxyListModel::filterAcceptsRow (int source_row, const QModelIndex & source_parent ) const
{
    MPListModel *source = dynamic_cast<MPListModel *>(sourceModel());
    if (!source)
        return false;
    
    QVariant t = source->data(source->index(source_row, 0));
    MPListItem *item = static_cast<MPListItem *>(t.value<void *>());
    MPAbstractListModelOps::DataType  dataType;
    dataType = getDataType();
    

    if (dataType == MPAbstractListModelOps::Album) {
        QString albumUrn = item->urn;
        QString albumID = AppWindow::instance()->settings.getAlbumID(albumUrn);
        QVector<QStringList> photosInTheAlbum;
        SearchEngine::getAlbumPhotos(albumID, photosInTheAlbum);
        bool isMatched = false;
        for (int i = 0; i < photosInTheAlbum.count(); i++) {
            if (matchPhoto(photosInTheAlbum[i][2], searchString)) {
                isMatched = true;
                break;
            }
        }
        return isMatched;
    } else if (dataType == MPAbstractListModelOps::Photo) {
        bool isMatched = matchPhoto(item->urn, searchString);
        return isMatched;

    }

    return false;
}

void MPProxyListModel::setSearchString(QString _searchString)
{
    searchString = _searchString;
    invalidateFilter();
}

void MPProxyListModel::setSpinner(const QModelIndex &index, bool showSpinner)
{
    MPAbstractListModelOps *ops = dynamic_cast<MPAbstractListModelOps *>(sourceModel());
    ops->setSpinner(mapToSource(index), showSpinner);
}

void MPProxyListModel::thumbnailWasLoaded(const QModelIndex &index)
{
    MPAbstractListModelOps *ops = dynamic_cast<MPAbstractListModelOps *>(sourceModel());
    ops->thumbnailWasLoaded(mapToSource(index));
}

void MPProxyListModel::setDefaultThumbnail(int row)
{
    MPAbstractListModelOps *ops = dynamic_cast<MPAbstractListModelOps *>(sourceModel());
    QModelIndex sourceIndex = index(row, 0, QModelIndex());
    ops->setDefaultThumbnail(sourceIndex.row());
}

void MPProxyListModel::addOutstandingThumbnailRequest(QString requestPath)
{
    MPAbstractListModelOps *ops = dynamic_cast<MPAbstractListModelOps *>(sourceModel());
    ops->addOutstandingThumbnailRequest(requestPath);
}
void MPProxyListModel::removeOutstandingThumbnailRequest(QString requestPath)
{
    MPAbstractListModelOps *ops = dynamic_cast<MPAbstractListModelOps *>(sourceModel());
    ops->addOutstandingThumbnailRequest(requestPath);
}

void MPProxyListModel::relayout()
{
    emit layoutChanged();
}
