/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */


#ifndef ALBUMSMODEL_H
#define ALBUMSMODEL_H

#include <MWidgetModel>

class M_EXPORT AlbumsModel : public MWidgetModel
{
    Q_OBJECT
    M_MODEL(AlbumsModel);
    M_MODEL_PROPERTY(QString, query, Query, true, QString());

private:
};

#endif // ALBUMSMODEL_H
