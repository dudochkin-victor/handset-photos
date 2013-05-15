/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */


#ifndef ABSTRACT_TASKLET_H
#define ABSTRACT_TASKLET_H

#include <QObject>
#include <QModelIndex>

#include <QDebug>

#include "mplistmodel.h"

struct MPListItem;
struct Job {
    QModelIndex row;
    QVariant userData;

    void dump();
};

class AbstractTasklet : public QObject
{
    Q_OBJECT
public:
    explicit AbstractTasklet(QObject *parent = 0);
    virtual ~AbstractTasklet();

    virtual void processSingleJob(Job& j) = 0;

    void addJob(Job& j) {jobs << j;}
signals:

public slots:
    void stopJobQueue();
    void processJobQueue();

private:
    QList<Job> jobs;
};

#endif // TASKLET_H
