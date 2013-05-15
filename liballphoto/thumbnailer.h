/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef THUMBNAILER_H
#define THUMBNAILER_H

#include <QtCore/QtCore>
#include <QtCore/QObject>
#include <QtDBus/QtDBus>

/// @class Thumbnailer
/// @brief Generates thumbnail images.
///
/// The Thumbnailer class:
/// - generates thumbnail images by using tumbler service.
///
/// The class is a singleton. There should only ever be one instance
/// running per application.
///
/// Thumbnails are generated asynchronously. A signal ready will be emitted
/// after thumbnail is stored on disk.
///
/// The thumbnail size is controlled by tumbler service.


class Thumbnailer: public QDBusAbstractInterface
{
    Q_OBJECT
public:
    /// @brief Return the instance of the thumbnailer singleton.
    /// @return Thumbnailer Returns the single instance.
    static Thumbnailer *instance();

    /// @brief Destructor
    ~Thumbnailer();

private:
    // Constructor is private because this is a singleton.
    // Construct it using the instance() method.
    Thumbnailer(const QDBusConnection &connection, QObject *parent = 0);

    /// @brief Tumbler DBUS service name
    const static QString service;

    /// @brief Tumbler DBUS service path
    const static QString path;

    /// @brief Tumbler DBUS service interface
    const static char* interface;

    /// @brief Tumbler default flavor
    const static QString default_flavor;

    /// @brief Tumbler default schedular
    const static QString default_schedular;

    static Thumbnailer *thumbnailerInstance;

public Q_SLOTS:
    /// @brief Request thumbnail using default flavor and schedular
    /// @param filePath const QString& The path of the video file
    /// @param mimetype const QString& The mime type of the video file
    /// @return QDBusPendingCall
    inline QDBusPendingCall requestThumbnail(const QString &filePath, const QString &mimetype) {
        QStringList paths;
        QStringList mimetypes;
        quint32 handle = 0;
        paths << filePath;
        mimetypes << mimetype;

        return asyncCall(QLatin1String("Queue"), paths, mimetypes,
                         default_flavor, default_schedular,
                         handle);
    }

    /// @brief Request thumbnail using specified flavor and schedular
    /// @param filePath const QString& The path of the video file
    /// @param mimetype const QString& The mime type of the video file
    /// @param flavor const QString& Specified flavor
    /// @param scheduler const QString& Specified scheduler
    /// @param handle quint32 Handler to dequeue from tumbler service
    /// @return QDBusPendingCall
    inline QDBusPendingCall requestThumbnail(const QString &filePath, const QString &mimetype,
                                             const QString &flavor, const QString &scheduler,
                                             quint32 handle) {
        QStringList paths;
        QStringList mimetypes;
        paths << filePath;
        mimetypes << mimetype;

        return asyncCall(QLatin1String("Queue"), paths, mimetypes,
                         flavor, scheduler,
                         handle);
    }

    /// @brief Request thumbnails for a bunch of video files using specified flavor and schedular
    /// @param filePath const QStringList& The paths of the video files
    /// @param mimetype const QStringList& The mime types of the video files
    /// @param flavor const QString& Specified flavor
    /// @param scheduler const QString& Specified scheduler
    /// @param handle quint32 Handler to dequeue from tumbler service
    /// @return QDBusPendingCall
    inline QDBusPendingCall requestThumbnail(const QStringList &paths, const QStringList &mimetypes,
                                             const QString &flavor, const QString &scheduler,
                                             quint32 handle) {
        return asyncCall(QLatin1String("Queue"), paths, mimetypes,
                         flavor, scheduler,
                         handle);
    }

    /// @brief Emit ready signals when DBUS signal Ready is received
    /// @param handle const unsigned int Handler that is ready
    /// @param urls const QStringList& Urls of video files that thumbnail is ready
    void emitReadySignal(const unsigned int &handle, const QStringList &urls);

    void emitErrorSignal(const unsigned int &handle, const QStringList &urls, const int &errorCode, const QString &message);

signals:
    /// @brief ready signals when thumbnails are ready
    /// @param handle const unsigned int Handler that is ready
    /// @param urls const QStringList& Urls of video files that thumbnail is ready
    void ready(const unsigned int &handle, const QStringList &urls);

    void error(const unsigned int &handle, const QStringList &urls, const int &errorCode, const QString &message);
};
#endif
