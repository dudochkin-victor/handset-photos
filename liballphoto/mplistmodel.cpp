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
#include "searchengine.h"
#include "thumbnailer.h"
#include "photostasklet.h"

QHash<QString, QString> MPListModel::urnToPathHashMap;

QString MPListModel::homePath = QDir::toNativeSeparators(QDir::homePath());
QString MPListModel::thumbnail_folder = QDir::separator() + QString(".thumbnails") + QDir::separator() + QString("normal");
QString MPListModel::thumbnail_suffix = ".png";

MPListModel::MPListModel(DynamicMList *controller) : list(controller)
{
    //Create Thumbnailer
    Thumbnailer::instance();
    QObject::connect(Thumbnailer::instance(), SIGNAL(ready(uint,QStringList)), this, SLOT(handleReady(uint,QStringList)), Qt::QueuedConnection);
    QObject::connect(Thumbnailer::instance(), SIGNAL(error(uint, QStringList, const int &, const QString &)), this, SLOT(handleError(uint,QStringList, const int &, const QString &)), Qt::QueuedConnection);
    //QObject::connect(TrackerListener::instance(), SIGNAL(SubjectsAdded(const QStringList &subjects)), this, SLOT(handlePhotoAdded(const QStringList &urns)));
    int indexValue = 0;

    //Build photoItemVector

    //defaultThumbnail.load(QString(MPListModel::testThumbnailDir).append("/defaultThumbnail.png"));
    pDefaultThumbnail = new QPixmap; //(QPixmap *)MTheme::pixmap("defaultThumbnail");
    //pDefaultThumbnail = new QPixmap(*defaultPixmap);
    //MTheme::releasePixmap(defaultPixmap);
     
#if 0
    // for debug only, pull all thumbnails int ~/.thumbnail into this model
    QDir thumbnailDir(MPListModel::testThumbnailDir);
    qDebug() << "dir is " << MPListModel::testThumbnailDir;
    bool yyy = thumbnailDir.exists();
    thumbnailDir.setNameFilters(QStringList() << "*.png");

    QFileInfoList fileInfoList = thumbnailDir.entryInfoList();
    QStringList xxx = thumbnailDir.entryList(QDir::Files);

    for (int i = 0; i < fileInfoList.size(); i++) {
        listItemsVector << new MPListItem("", fileInfoList.at(i).absoluteFilePath(), defaultThumbnail);
    }
#endif
        thumbnailDir = new QDir(homePath + thumbnail_folder);
    
    //get all photo urls from tracker
    QVector<QStringList> *photos = NULL;   //hold all photo urls got from tracker
    bool deletePhotos = false;
        //get all photo urls from tracker
            photos = new QVector<QStringList>();
            deletePhotos = true;
            SearchEngine::getAllPhoto(*photos);

    for (QVector<QStringList>::iterator i = photos->begin(); i != photos->end(); i++) {
        QFile f;
        QUrl url = QUrl::fromEncoded((*i)[0].toAscii());

        f.setFileName(url.toLocalFile());
        if (f.exists()) // only create photoItem for files that exist; Tracker may has bug
        {
            MPListItem *item = new MPListItem( (*i)[0] , "", pDefaultThumbnail, (*i)[1]);
            item->urn = (*i)[2];
            listItemsVector << item;

            pathToIndexHashMap.insert(url.toLocalFile(), indexValue);
            urnToPathHashMap.insert(item->urn, url.toLocalFile());
            indexValue++;
        }
    }

    if (deletePhotos)
        delete photos;
}

void MPListModel::addOutstandingThumbnailRequest(QString requestPath)
{
    outstandingThumbnailRequest.insert(requestPath);
}

void MPListModel::removeOutstandingThumbnailRequest(QString requestPath)
{
    outstandingThumbnailRequest.remove(requestPath);
}



void MPListModel::handleReady(const unsigned int &handler, const QStringList &urls)
{
    for (QStringList::const_iterator i = urls.begin(); i != urls.end(); i++) {
        QString localPath = *i;
        QString key = localPath.remove(QRegExp("^file:\/\/"));
        if (!outstandingThumbnailRequest.contains(key)) {
            outstandingThumbnailRequest.remove(key);
            continue;
        }
        int indexValue = pathToIndexHashMap.value(key, -1);
        if (indexValue == -1)
            continue;
        MPListItem *entry = listItemsVector[indexValue];
        if (entry->thumbnailURI.isEmpty())
            continue;
        entry->thumbnail = new QPixmap(PhotosTasklet::squareQPixmapFromPath(entry->thumbnailURI, list->cellCreator()->cellSize().toSize().width()));
        entry->isLoaded = true;
        entry->showSpinner = false;
        QModelIndex modelIndex = index(indexValue, 0);
        QModelIndex next = modelIndex.sibling(modelIndex.row() + 1, 0);
        if (next.isValid()) {
            QVariant data = next.data(Qt::DisplayRole);
            MPListItem *next_entry = static_cast<MPListItem *>(data.value<void *>());
            if (!next_entry->isLoaded)
                next_entry->showSpinner = true;
            thumbnailWasLoaded(next);   //emit date changed signal;
        }
 
        thumbnailWasLoaded(modelIndex);
        QTimer::singleShot(0, list->getTasklet(), SLOT(processJobQueue()));
    }
}

void MPListModel::handleError(const unsigned int &handler, const QStringList &urls, const int &errorCode, const QString &message)
{
    if (errorCode == 0)
        return;
    for (QStringList::const_iterator i = urls.begin(); i != urls.end(); i++) {
        QString localPath = *i;
        QString key = localPath.remove(QRegExp("^file:\/\/"));
        if (!outstandingThumbnailRequest.contains(key)) {
            outstandingThumbnailRequest.remove(key);
            continue;
        }
        int indexValue = pathToIndexHashMap.value(key, -1);
        if (indexValue == -1)
            continue;
        MPListItem *entry = listItemsVector[indexValue];
        entry->thumbnail = new QPixmap(PhotosTasklet::squareQPixmapFromPath(entry->thumbnailURI, list->cellCreator()->cellSize().toSize().width()));
        entry->isLoaded = true;
        entry->showSpinner = false;
        QModelIndex modelIndex = index(indexValue, 0);
        QModelIndex next = modelIndex.sibling(modelIndex.row() + 1, 0);
        if (next.isValid()) {
            QVariant data = next.data(Qt::DisplayRole);
            MPListItem *next_entry = static_cast<MPListItem *>(data.value<void *>());
            if (!next_entry->isLoaded) {
                next_entry->showSpinner = true;
            }
            thumbnailWasLoaded(next);   //emit date changed signal;
        }
 
        thumbnailWasLoaded(modelIndex);
        QTimer::singleShot(0, list->getTasklet(), SLOT(processJobQueue()));
    }
}

MPListModel::~MPListModel()
{
    QObject::disconnect(Thumbnailer::instance(), SIGNAL(ready(uint,QStringList)), this, SLOT(handleReady(uint,QStringList)));
    QObject::disconnect(Thumbnailer::instance(), SIGNAL(error(uint, QStringList, const int &, const QString &)), this, SLOT(handleError(uint,QStringList, const int &, const QString &)));
    for (int i = 0; i < listItemsVector.count(); i++) {
        delete listItemsVector[i];
    }

    listItemsVector.resize(0);
    delete thumbnailDir;
    //MTheme::releasePixmap(pDefaultThumbnail);
    delete pDefaultThumbnail;
}

QString MPListModel::testThumbnailDir = "themes/.thumbnails/normal";

QVariant MPListModel::data(const QModelIndex &index, int role) const
{
    // This function will be called many times during fast panning, lets
    // check boundaries and validnes only in debug mode
    Q_ASSERT(index.isValid());
    Q_ASSERT(index.row() < listItemsVector.size());

    if (role == Qt::DisplayRole) {
        // Let's store a pointer into QVariant, otherwise QT will make a copy but we are lazy
        // and don't want to copy stuff around
        return QVariant::fromValue(static_cast<void *>(listItemsVector[index.row()]));
    }

    return QVariant();
}

int MPListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return listItemsVector.size();
}

int MPListModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return 1;
}

/*
bool MPListModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if (count <= 0)
        return true;

    emit layoutAboutToBeChanged();
    beginInsertRows(parent, row, row + count - 1);
    for (int i=0; i < count; i++) {
        MPListItem *item = listItemsVector[row + i];
        QString fileString = item->photoURI.remove(QRegExp("^file:\/\/"));
        QStringList list = fileString.split("/");
        QString filename = list.last();
        
        QString targetName = "copy_of_" + filename;
        list[list.count()-1] = targetName;
        QString targetString = list.join("/");
        int n = 2;
        while (QFile::exists(targetString)) {
            targetName = "copy_" + QString::number(n) + "_of_" + filename;
            list[list.count()-1] = targetName;
            targetString = list.join("/");
            n++;
        }
        QFile::copy(fileString, targetString);
    }
    endInsertRows();
    emit layoutChanged();
    return true;
}
*/


/*
bool MPListModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (count <= 0)
        return true; //Successfully removed 0 rows.

    emit layoutAboutToBeChanged();
    beginRemoveRows(parent, row, row + count - 1);
    //Todo: Huan review this for possible tracker-related clean-up
    for (int i=0; i < count; i++) {
        MPListItem *itemToBeDeleted = listItemsVector[row + i];
        QFile::remove(itemToBeDeleted->photoURI.remove(QRegExp("^file:\/\/")));
        delete itemToBeDeleted;
        QUrl url = QUrl::fromEncoded(listItemsVector[row]->photoURI.toAscii());
        pathToIndexHashMap.remove(url.toLocalFile());
        listItemsVector.remove(row);

    }
    endRemoveRows();
    emit layoutChanged();
    return true;
}
*/


void MPListModel::thumbnailWasLoaded(const QModelIndex &index)
{
    emit dataChanged(index, index);
}

void MPListModel::setSparql(QString qlString)
{
    //rebuild the model with the new qlString
}

void MPListModel::setDefaultThumbnail(int index)
{
    if (listItemsVector[index]->isLoaded) {
        delete listItemsVector[index]->thumbnail;
        listItemsVector[index]->thumbnail = pDefaultThumbnail;
        listItemsVector[index]->isLoaded = false;
    }
}

void MPListModel::setSpinner(const QModelIndex &index, bool showSpinner)
{
    int i = index.row();
    if (listItemsVector[i]->isLoaded && !showSpinner)
        listItemsVector[i]->showSpinner = false;
    else if (!listItemsVector[i]->isLoaded)
        listItemsVector[i]->showSpinner = showSpinner;

    thumbnailWasLoaded(index);
}

#if 0
void MPListModel::updateAlbumModel()
{
    qDebug() << "MPListModel::updateAlbumModel()";
    if (getDataType() == MPAbstractListModelOps::Album) {
        // create a album model
        QVector<QStringList> *result = new QVector<QStringList>();
        SearchEngine::getAllPhotoAlbums(*result);
        qDebug() << "MPListModel::updateAlbumModel() getallphotoalbums:" << t.elapsed();
        emit layoutAboutToBeChanged();
        beginRemoveRows(QModelIndex(), 0, listItemsVector.count() - 1);
        qDebug() << "albums:" << result->size();
        for (int i = 0; i < listItemsVector.count(); i++) {
            delete listItemsVector[i];
        }
        listItemsVector.clear();
        pathToIndexHashMap.clear();
        endRemoveRows();

        int indexValue = 0;
        MPSettings settings;
        for(QVector<QStringList>::iterator i = result->begin();i != result->end(); i++) {
            MPListItem *t = new MPListItem((*i)[1], ((*i)[2]).toInt(), (*i)[0]);
            t->thumbnail = pDefaultThumbnail;
            t->urn = (*i)[0];
            qDebug() << "paul debug urn in updateAlbum:" << t->urn;
            t->photoURI = settings.getAlbumCover(t->urn);
            qDebug() << "12345 paul debug cover uri:" << t->photoURI;
            listItemsVector << t;
            qDebug() << "paul debug in update album " << t->urn << "num photos" << (*i)[2];

            pathToIndexHashMap.insert(QUrl::fromEncoded(t->photoURI.toAscii()).toLocalFile(), indexValue++);
        }
        emit layoutChanged();
        emit dataChanged(index(0, 0), index(result->size() -1, 0));
        reset();
        list->getTasklet()->stopJobQueue();
        list->doTasklet();
    }
    qDebug() << "MPListModel::updateAlbumModel() end";
}
#else

#endif

void MPListModel::relayout()
{
    emit layoutChanged();
}

