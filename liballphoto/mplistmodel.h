/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */


#ifndef MPLISTMODEL_H
#define MPLISTMODEL_H

#include <QAbstractTableModel>
#include <QPixmap>
#include "thumbnailer.h"
#include "dynamicmlist.h"

// Structure which contain data for each item 
// This structure is used for both photo and album
struct MPListItem {
public:
    MPListItem(QString newphotoURI, QString newThumbnailURI, QPixmap *t, QString newmimetype) :
            thumbnail(NULL), showSpinner(false), photoURI(newphotoURI), thumbnailURI(newThumbnailURI), isLoaded(false), mimetype(newmimetype), numPhotos(0)
    {
        thumbnail = t;   //implicit data sharing
    }

    MPListItem(QString _albumName, int _numPhotos) :
        thumbnail(NULL), showSpinner(false), isLoaded(false), numPhotos(_numPhotos) {};

    MPListItem(QString _albumName, int _numPhotos, QString _albumID) :
        thumbnail(NULL), showSpinner(false), isLoaded(false), albumID(_albumID), numPhotos(_numPhotos) {};


    QString urn;


    QPixmap *thumbnail; // will be filled with empty image, after real image will be loaded it will replace old one
    bool showSpinner;   //whether to show Spinner
    QString photoURI;
    QString thumbnailURI;
    bool isLoaded;
    QString mimetype;

//    QString albumName;  //album Name. for album item only
    QString albumID;    // used for backend to idenity album
    int numPhotos;      // number of photos in this album. for album item only

private:
 //   explicit MPListItem();   //disable default constructor
};

class DynamicMList;
//Data model for photo listing and album listing
class MPListModel: public QAbstractTableModel
{
    Q_OBJECT

public:
    // Defining roles here which will be used for sorting and filtering in PhoneBookSortedModel
    //enum PhoneBookRoles {
    //    PhoneBookSortRole = Qt::UserRole + 1,
    //    PhoneBookFilterRole
    //};
    MPListModel(DynamicMList *controller);
    virtual ~MPListModel();

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    void setSparql(QString qlString);
    void removeAlbum(const QModelIndex &index);

/*    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
*/
    virtual void thumbnailWasLoaded(const QModelIndex &index);
    virtual void setDefaultThumbnail(int index);
    virtual void setSpinner(const QModelIndex &index, bool showSpinner);

    virtual void addOutstandingThumbnailRequest(QString ruquestPath);
    virtual void removeOutstandingThumbnailRequest(QString ruquestPath);

    virtual void relayout();

    static QString testThumbnailDir;
    static QString homePath;
    static QString thumbnail_folder;
    static QString thumbnail_suffix;
    Thumbnailer *thumbnailer;
    QDir *thumbnailDir;

private:
    QVector<MPListItem *> listItemsVector;
    QPixmap *pDefaultThumbnail;
    QHash<QString, int> pathToIndexHashMap;
    static QHash<QString, QString> urnToPathHashMap;
    QSet<QString> outstandingThumbnailRequest;
    DynamicMList *list;

    friend class DynamicPhotoList;
    friend class DynamicAlbumList;
private slots:
    void handleReady(const unsigned int &handler, const QStringList &urls);
    void handleError(const unsigned int &handler, const QStringList &urls, const int &errorcode, const QString &message);
};



#endif // MPLISTMODEL_H
