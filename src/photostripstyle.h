/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */


#ifndef PHOTOSTRIPSTYLE_H
#define PHOTOSTRIPSTYLE_H

#include <mwidgetstyle.h>

class M_EXPORT PhotoStripStyle : public MWidgetStyle
{
    Q_OBJECT
    M_STYLE(PhotoStripStyle)
    M_STYLE_ATTRIBUTE(int, snapInterval, SnapInterval)
};

class M_EXPORT PhotoStripStyleContainer : public MWidgetStyleContainer
{
    M_STYLE_CONTAINER(PhotoStripStyle)
};

#endif // PHOTOSTRIPSTYLE_H
