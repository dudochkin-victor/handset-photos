/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */


#include <QDebug>
#include <QFile>

#include "photolistdeleteconfirm.h"

void PhotoListDeleteConfirm::onAccepted()
{
    qDebug() << "User accepts ";
    QFile::remove(file);
}


void PhotoListDeleteConfirm::setFile(QString fileToBeDeleted)
{
    file = fileToBeDeleted;
}

void PhotoListDeleteConfirm::onRejected()
{
    qDebug() << "User rejects ";
}

