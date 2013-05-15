/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef TRACKERLISTENER_H
#define TRACKERLISTENER_H

#include <QtCore/QtCore>
#include <QtCore/QObject>
#include <QtDBus/QtDBus>


class TrackerListener: public QDBusAbstractInterface
{
    Q_OBJECT
public:

    static TrackerListener *instance();

    /// @brief Destructor
    ~TrackerListener();

private:
    // Constructor is private because this is a singleton.
    // Construct it using the instance() method.
    TrackerListener(const QDBusConnection &connection, QObject *parent = 0);

    /// @brief Tumbler DBUS service name
    const static QString service;

    /// @brief Tumbler DBUS service path
    const static QString path;

    /// @brief Tumbler DBUS service interface
    const static char* interface;

    static TrackerListener *trackerListenerInstance;

public Q_SLOTS:


    void emitSubjectsChangedSignal(const QStringList &subject, const QStringList &predicates);

    void emitSubjectsAddedSignal(const QStringList &subjects);

    void emitSubjectsRemovedSignal(const QStringList &subjects);

signals:

    void SubjectsChanged(const QStringList &subject, const QStringList &predicates);

    void SubjectsAdded(const QStringList &subjects);

    void SubjectsRemoved(const QStringList &subjects);
};
#endif
