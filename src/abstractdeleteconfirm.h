/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */


#ifndef ABSTRACT_DELETE_CONFIRM
#define ABSTRACT_DELETE_CONFIRM
#include <QObject>
//an abstract interface to be called when user
//clicks Yes or No of the  delte confirmation dialog
//
class AbstractDeleteConfirm : public QObject{
    Q_OBJECT
public slots:
    virtual void onAccepted() = 0;
    virtual void onRejected() = 0;
};

#endif
