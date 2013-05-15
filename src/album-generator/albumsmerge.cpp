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

#include "searchengine.h"
#include "albumsmerge.h"
#include "../appwindow.h"

void albumsMerge::storeAlbumNameID(QString urn, QString albumID, QString albumName)
{
    QStringList value;
    value << albumID << albumName;
    AppWindow::instance()->settings.setValue(urn, value);
}

bool albumsMerge::Merge(QVector<QStringList> *photos)
{

     int interval = 0;
     QVector<int> *intervalVector = new QVector<int>();

     /*delete the album in the tracker data for the new merging*/
	 for(int i =1 ; i<21; i++)
     {
      //   qDebug() << "deleteAlbum " << i; 
         SearchEngine::deleteAlbum("album"+QString::number(i,10));
     }
     for (QVector<QStringList>::iterator i = photos->begin(); i != photos->end(); i++) {
         QUrl url = QUrl::fromEncoded((*i)[0].toAscii());
	     if( !QFileInfo(url.toLocalFile()).created().date().toString().isEmpty()) {
         	 interval = -QDate::currentDate().daysTo((QFileInfo(url.toLocalFile()).created().date()));
             if(!intervalVector->contains(interval))
                 intervalVector->append(interval);/*get the interval date from current date*/
         }
     }

     /*sort the date interval*/
     int temp = 0;

     for (QVector<int>::iterator i = intervalVector->begin(); i != intervalVector->end(); i++) {
         int flag = 0 ;
         for(QVector<int>::iterator j = i; j != intervalVector->end(); j++) {
             if(*j < *(j+1)){
                 temp = *j;
                 *j = *(j+1);
                 *(j+1) = temp;
                 flag = 1;
              }
          }
        if (flag == 0)
		    break;
    }
    
     /*create the albums not more than MaxAlbum(20)*/ 
     
	  int intervalNum = 1/*, counterSum = 0*/;
      QStringList lastAlbumphotoUrns;
    
      for (QVector<int>::iterator j = intervalVector->begin(); j != intervalVector->end(); j++) {
          int counter= 0;
          QStringList photoUrns;
		  QString coverUrl;
		  int flag = 1;
          for (QVector<QStringList>::iterator i = photos->begin(); i != photos->end(); i++) {
              QUrl url = QUrl::fromEncoded((*i)[0].toAscii());
              if( !QFileInfo(url.toLocalFile()).created().date().toString().isEmpty()) {
		    	  if(QDate::currentDate().daysTo((QFileInfo(url.toLocalFile()).created().date()))== -(*j) ) {
					 if(intervalNum < MaxAlbums){
				          photoUrns << (*i)[2];
                          counter++;
					  	  qDebug()<<"photoUrns:"<<photoUrns;
						  if(flag ==1)
							  {
								coverUrl = url.toLocalFile(); 
								flag = 0;
					          }
						  }
                      else {
                          lastAlbumphotoUrns <<(*i)[2];
					      counter++;
						  if(flag ==1)
                              {
								coverUrl = url.toLocalFile(); 
                             	flag = 0;
							  }
                          }

                      }
                  }
              }
          if(counter != 0) {
              QString albumID = "album"+QString::number(intervalNum, 10);
              if((intervalVector->count()) < MaxAlbums) {
                  if(*j == 0){
                      SearchEngine::createAlbum(albumID, "Today", photoUrns);//if the date is today ,set the album title is "Today"
                      //QVector<QStringList> *albumPhotos = new QVector<QStringList>();
                      //SearchEngine::getAlbumPhotos("album"+QString::number(intervalNum, 10), *albumPhotos);
                      //QUrl url = QUrl::fromEncoded((*(albumPhotos)[0]).toAscii());
                      for(int i = 0; i < photoUrns.size(); i++)
                          storeAlbumNameID(photoUrns.at(i), albumID, "Today");

                      QString AlbumURN;
                      SearchEngine::getAlbumURN("album"+QString::number(intervalNum, 10), AlbumURN);
                      qDebug() << "AlbumURN " << AlbumURN;
                      SearchEngine::setAlbumCover(AlbumURN, coverUrl);
                      //SearchEngine::setAlbumCover(photoUrns[0], coverUrl);
                  }
                  if(*j == 1) {
                      SearchEngine::createAlbum(albumID, "Yesterday", photoUrns);//if the date is yesterday,set the album title is "Yesterday"
                      //QVector<QStringList> *albumPhotos = new QVector<QStringList>();
                      //SearchEngine::getAlbumPhotos("album"+QString::number(intervalNum, 10), *albumPhotos);
                      //QUrl url = QUrl::fromEncoded((*(albumPhotos)[0]).toAscii());
                      for(int i = 0; i < photoUrns.size(); i++)
                          storeAlbumNameID(photoUrns.at(i), albumID, "Yesterday");

                      QString AlbumURN;
                      SearchEngine::getAlbumURN(albumID, AlbumURN);
                      qDebug() << "AlbumURN " << AlbumURN;
                      SearchEngine::setAlbumCover(AlbumURN, coverUrl);
                      //SearchEngine::setAlbumCover(photoUrns[0], coverUrl);
                  }

                  if(*j > 1) {
                      SearchEngine::createAlbum(albumID, QDate::currentDate().addDays(-(*j)).toString(), photoUrns);// if the date is more than two days ,set the album tite is the date time
                      //QVector<QStringList> *albumPhotos = new QVector<QStringList>();
                      //SearchEngine::getAlbumPhotos("album"+QString::number(intervalNum, 10), *albumPhotos);
                      //QUrl url = QUrl::fromEncoded(((*(albumPhotos->begin()))[0]).toAscii());
                      for(int i = 0; i < photoUrns.size(); i++)
                          storeAlbumNameID(photoUrns.at(i), albumID, QDate::currentDate().addDays(-(*j)).toString());

                      QString AlbumURN;
                      SearchEngine::getAlbumURN(albumID, AlbumURN);
                      qDebug() << "AlbumURN " << AlbumURN;
                      SearchEngine::setAlbumCover(AlbumURN, coverUrl);
                      //SearchEngine::setAlbumCover(photoUrns[0], coverUrl);
                  }
              } else {
                  if(intervalNum < MaxAlbums) {
                      if(*j == 0) {
                          SearchEngine::createAlbum(albumID, "Today", photoUrns);
                          //QVector<QStringList> *albumPhotos = new QVector<QStringList>();
                          //SearchEngine::getAlbumPhotos("album"+QString::number(intervalNum, 10), *albumPhotos);
                          //QUrl url = QUrl::fromEncoded(((*(albumPhotos->begin()))[0]).toAscii());
                          for(int i = 0; i < photoUrns.size(); i++)
                              storeAlbumNameID(photoUrns.at(i), albumID, "Today");

                          QString AlbumURN;
                          SearchEngine::getAlbumURN(albumID, AlbumURN);
                          qDebug() << "AlbumURN " << AlbumURN;
                          SearchEngine::setAlbumCover(AlbumURN, coverUrl);
                          // SearchEngine::setAlbumCover(photoUrns[0], coverUrl);
                      }
                      if(*j == 1) {
                          SearchEngine::createAlbum(albumID, "Yesterday", photoUrns);
                          // QVector<QStringList> *albumPhotos = new QVector<QStringList>();
                          //SearchEngine::getAlbumPhotos("album"+QString::number(intervalNum, 10), *albumPhotos);
                          //QUrl url = QUrl::fromEncoded(((*(albumPhotos->begin()))[0]).toAscii());
                          for(int i = 0; i < photoUrns.size(); i++)
                              storeAlbumNameID(photoUrns.at(i), albumID, "Yesterday");
                          QString AlbumURN;
                          SearchEngine::getAlbumURN(albumID, AlbumURN);
                          qDebug() << "AlbumURN " << AlbumURN;
                          SearchEngine::setAlbumCover(AlbumURN, coverUrl);
                          //SearchEngine::setAlbumCover(photoUrns[0], coverUrl);
                      }
                      if(*j > 1) {
                          SearchEngine::createAlbum(albumID, QDate::currentDate().addDays(-(*j)).toString(), photoUrns);
                          // QVector<QStringList> *albumPhotos = new QVector<QStringList>();
                          //SearchEngine::getAlbumPhotos("album"+QString::number(intervalNum, 10), *albumPhotos);
                          //QUrl url = QUrl::fromEncoded(((*(albumPhotos->begin()))[0]).toAscii());
                          for(int i = 0; i < photoUrns.size(); i++)
                              storeAlbumNameID(photoUrns.at(i), albumID, QDate::currentDate().addDays(-(*j)).toString());

                          QString AlbumURN;
                          SearchEngine::getAlbumURN(albumID, AlbumURN);
                          qDebug() << "AlbumURN " << AlbumURN;
                          SearchEngine::setAlbumCover(AlbumURN, coverUrl);
                          // SearchEngine::setAlbumCover(photoUrns[0], coverUrl);
                      }
                  }
                  else {
                      SearchEngine::createAlbum("album20",QString("Before ")+ QDate::currentDate().addDays(-(*j)).toString(), lastAlbumphotoUrns);//if the date scope is more than twenty ,the last one album will contain all the photos which is before the twentieth date time
                      //QVector<QStringList> *albumPhotos = new QVector<QStringList>();
                      //SearchEngine::getAlbumPhotos("album"+QString::number(intervalNum, 10), *albumPhotos);
                      //QUrl url = QUrl::fromEncoded(((*(albumPhotos->begin()))[0]).toAscii());
                      for(int i = 0; i < photoUrns.size(); i++)
                          storeAlbumNameID(photoUrns.at(i), "album20", QDate::currentDate().addDays(-(*j)).toString());


                      QString AlbumURN;
                      SearchEngine::setAlbumCover("album20", coverUrl);
                      //SearchEngine::setAlbumCover(lastAlbumphotoUrns[0], coverUrl);
                      return TRUE ;

                  }
              }
          }
          intervalNum++;
       }
       
   return TRUE;
}
