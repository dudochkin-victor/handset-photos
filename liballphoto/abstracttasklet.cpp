/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */


#include <QTimer>
#include "abstracttasklet.h"

AbstractTasklet::AbstractTasklet(QObject *parent) :
    QObject(parent)
{
    Q_UNUSED(parent);
}

AbstractTasklet::~AbstractTasklet()
{
    jobs.clear();
}


void AbstractTasklet::stopJobQueue()
{
    jobs.clear();
}

void AbstractTasklet::processJobQueue()
{
    if (jobs.isEmpty())
        return;

    Job job = jobs.takeFirst();
//    job.dump();
    processSingleJob(job);

/*    if(jobs.count() > 0)
        QTimer::singleShot(0, this, SLOT(processJobQueue())); */
}

void Job::dump()
{
    qDebug() << "entering dump";
    MPListItem *entry = static_cast<MPListItem *>(userData.value<void *>());
    qDebug() << "entry is " << entry;
    qDebug() << "xxxx: " << entry->thumbnailURI;
    qDebug() << "Job@row " << row.row() << ":" << entry->thumbnailURI;
    qDebug() << "leaving dump";
}

