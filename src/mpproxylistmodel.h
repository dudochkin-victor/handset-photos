/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef MP_PROXY_LIST_MODEL_H
#define MP_PROXY_LIST_MODEL_H
#include <QSortFilterProxyModel>
#include "mpabstractlistmodelops.h"


class MPProxyListModel : public QSortFilterProxyModel, public MPAbstractListModelOps {
    Q_OBJECT
public:
    MPProxyListModel(DataType type, QString _searchString = "", QObject *parent = 0) : searchString(_searchString), 
    QSortFilterProxyModel(parent), MPAbstractListModelOps(type) {};
    virtual ~MPProxyListModel() {};

    void setSearchString(QString _searchString);

    virtual void setSpinner(const QModelIndex &index, bool showSpinner);
    virtual void thumbnailWasLoaded(const QModelIndex &index);
    virtual void setDefaultThumbnail(int index);
    virtual void addOutstandingThumbnailRequest(QString ruquestPath);
    virtual void removeOutstandingThumbnailRequest(QString ruquestPath);

    virtual void relayout();

protected:
    virtual bool filterAcceptsRow(int source_row, const QModelIndex & source_parent) const;

private:
    void parseDate(QString in, int &year, int &month, int &day, int &dayOfAWeek) const;
    bool compareDate(const QString &dateString1, const QString &userDateString) const;
    bool compareLocation(const QString &locationString1, const QString &userLocationString) const;
    bool matchPhoto(const QString &photoUrn, const QString &userInput) const;
    QString searchString;
};

#endif
