/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */


#include "trackerlistener.h"

const QString TrackerListener::service = "org.freedesktop.Tracker1";
const QString TrackerListener::path = "/org/freedesktop/Tracker1/Resources/Classes/nfo/Image";
const char   *TrackerListener::interface = "org.freedesktop.Tracker1.Resources.Class";

TrackerListener *TrackerListener::trackerListenerInstance = NULL;

TrackerListener *TrackerListener::instance()
{
    if (trackerListenerInstance)
        return trackerListenerInstance;
    else {
        trackerListenerInstance = new TrackerListener(QDBusConnection::sessionBus(), NULL);
        return trackerListenerInstance;
    }
}


void TrackerListener::emitSubjectsChangedSignal(const QStringList &subject, const QStringList &predicates)
{
    qDebug() << "TrackerListener::emitSubjectsChangedSignal " << subject << " " << predicates;
    emit SubjectsChanged(subject, predicates);
}

void TrackerListener::emitSubjectsAddedSignal(const QStringList &subjects)
{
    qDebug() << "TrackerListener::emitSubjectsAddedSignal " << subjects;
    emit SubjectsAdded(subjects);
}

void TrackerListener::emitSubjectsRemovedSignal(const QStringList &subjects)
{
    qDebug() << "TrackerListener::emitSubjectsRemovedSignal " << subjects;
    emit SubjectsRemoved(subjects);
}


TrackerListener::TrackerListener(const QDBusConnection &connection, QObject *parent) :
        QDBusAbstractInterface(service, path, interface, connection, parent)
{

    qDebug() << "TrackerListener try to connect SubjectsChanged ";
    QDBusConnection::sessionBus().connect(service, path, interface,
                                    "SubjectsChanged", this, SLOT(emitSubjectsChangedSignal(const QStringList, const QStringList)));

    qDebug() << "TrackerListener try to connect SubjectsAdded ";
    QDBusConnection::sessionBus().connect(service, path, interface,
                                    "SubjectsAdded", this, SLOT(emitSubjectsAddedSignal(const QStringList)));

    qDebug() << "TrackerListener try to connect SubjectsRemoved ";
    QDBusConnection::sessionBus().connect(service, path, interface,
                                    "SubjectsRemoved", this, SLOT(emitSubjectsRemovedSignal(const QStringList)));
}

TrackerListener::~TrackerListener()
{
    qDebug() << "TrackerListener::~TrackerListener()";
}
