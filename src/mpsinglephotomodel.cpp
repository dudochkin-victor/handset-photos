/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */


#include <QDir>
#include <QDebug>
#include <QtDBus/QtDBus>
#include <MTheme>
#include "mplistmodel.h"
#include "photostasklet.h"
#include "appwindow.h"
#include "trackerlistener.h"
#include "mpsettings.h"
#include "mpsinglephotomodel.h"
#include "appwindow.h"

MPSinglePhotoModel::MPSinglePhotoModel(const QString &photoPath)
{
    MPListItem *item = new MPListItem(QString("file://").append(photoPath) , "", NULL, "");
    listItemsVector << item;
}

MPSinglePhotoModel::~MPSinglePhotoModel()
{
    for (int i = 0; i < listItemsVector.count(); i++) {
        delete listItemsVector[i];
    }

    listItemsVector.resize(0);
}

QVariant MPSinglePhotoModel::data(const QModelIndex &index, int role) const
{
    // This function will be called many times during fast panning, lets
    // check boundaries and validnes only in debug mode
    Q_ASSERT(index.isValid());
    if (index.row() >= listItemsVector.size())
    Q_ASSERT(index.row() < listItemsVector.size());

    if (role == Qt::DisplayRole) {
        // Let's store a pointer into QVariant, otherwise QT will make a copy but we are lazy
        // and don't want to copy stuff around
        return QVariant::fromValue(static_cast<void *>(listItemsVector[index.row()]));
    }

    return QVariant();
}

int MPSinglePhotoModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return listItemsVector.size();
}

int MPSinglePhotoModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return 1;
}

