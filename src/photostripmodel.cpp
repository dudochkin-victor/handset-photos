/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */


#include "photostripmodel.h"


void PhotoStripModel::setStripIndex(const QModelIndex &index)
{
    _stripIndex() =  index;
    memberModified(StripIndex);
}

void PhotoStripModel::setSlideShow(const bool &on)
{
    _slideShow() = on;
    memberModified(SlideShow);
}

void PhotoStripModel::setStripSmall(const bool &on)
{
    _stripSmall() = on;
    memberModified(StripSmall);
}

const QModelIndex &PhotoStripModel::stripIndex() const
{
    return _stripIndex();
}

const bool &PhotoStripModel::slideShow() const
{
    return _slideShow();
}

const bool &PhotoStripModel::stripSmall() const
{
    return _stripSmall();
}
