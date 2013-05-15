/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */


#ifndef MPSINGLEMODEL_H
#define MPSINGLEMODEL_H

#include <QAbstractTableModel>
#include <QPixmap>
#include "thumbnailer.h"
#include "dynamicmlist.h"
#include "mpabstractlistmodelops.h"
#include "mpsettings.h"
#include "mplistmodel.h"

// Structure which contain data for each item 
// This structure is used for both photo and album

class DynamicMList;
//Data model for photo listing and album listing
class MPSinglePhotoModel: public QAbstractTableModel
{
    Q_OBJECT

public:
    // Defining roles here which will be used for sorting and filtering in PhoneBookSortedModel
    //enum PhoneBookRoles {
    //    PhoneBookSortRole = Qt::UserRole + 1,
    //    PhoneBookFilterRole
    //};
    MPSinglePhotoModel(const QString &photoPath);
    virtual ~MPSinglePhotoModel();

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

private:
    QVector<MPListItem *> listItemsVector;
};



#endif // MPLISTMODEL_H
