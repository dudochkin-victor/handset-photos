/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */


#ifndef PHOTOSMODEL_H
#define PHOTOSMODEL_H

#include <MWidgetModel>

class M_EXPORT PhotosModel : public MWidgetModel
{
    Q_OBJECT
    M_MODEL(PhotosModel);
    M_MODEL_PROPERTY(QString, query, Query, true, QString());

private:
};

#endif // PHOTOSMODEL_H
