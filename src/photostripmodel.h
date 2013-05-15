/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */


#ifndef PHOTOSTRIPMODEL_H
#define PHOTOSTRIPMODEL_H

#include <MWidgetModel>
#include <QModelIndex>

class M_EXPORT PhotoStripModel : public MWidgetModel
{
    Q_OBJECT
    M_MODEL(PhotoStripModel);
    M_MODEL_PROPERTY(QModelIndex, stripIndex, StripIndex, false, QModelIndex());
    M_MODEL_PROPERTY(bool, slideShow, SlideShow, false, false);
    M_MODEL_PROPERTY(bool, stripSmall, StripSmall, false, false);

private:
};

#endif // PHOTOSTRIPMODEL_H
