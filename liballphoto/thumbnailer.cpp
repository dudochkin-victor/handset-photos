/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */


#include "thumbnailer.h"

const QString Thumbnailer::service = "org.freedesktop.thumbnails.Thumbnailer1";
const QString Thumbnailer::path = "/org/freedesktop/thumbnails/Thumbnailer1";
const char   *Thumbnailer::interface = "org.freedesktop.thumbnails.Thumbnailer1";

const QString Thumbnailer::default_flavor = "normal";
const QString Thumbnailer::default_schedular = "foreground";

Thumbnailer *Thumbnailer::thumbnailerInstance = NULL;

Thumbnailer *Thumbnailer::instance()
{
    if (thumbnailerInstance)
        return thumbnailerInstance;
    else {
        thumbnailerInstance = new Thumbnailer(QDBusConnection::sessionBus(), NULL);
        return thumbnailerInstance;
    }
}

void Thumbnailer::emitReadySignal(const unsigned int &handle, const QStringList &urls)
{
    emit ready(handle, urls);
}

void Thumbnailer::emitErrorSignal(const unsigned int &handle, const QStringList &urls, const int &errorCode, const QString &message)
{
    qDebug() << "ERROR SIGNAL " << handle << urls << errorCode << message;
    emit error(handle, urls, errorCode, message);
}

Thumbnailer::Thumbnailer(const QDBusConnection &connection, QObject *parent) :
        QDBusAbstractInterface(service, path, interface, connection, parent)
{
    QDBusConnection::sessionBus().connect(service, path, interface,
                                    "Ready", this, SLOT(emitReadySignal(const unsigned int, const QStringList)));
    QDBusConnection::sessionBus().connect(service, path, interface,
                                    "Error", this, SLOT(emitErrorSignal(const unsigned int, const QStringList, \
                                                                        const int, const QString )));
}

Thumbnailer::~Thumbnailer()
{
}
