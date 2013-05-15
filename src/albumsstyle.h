/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */


#ifndef ALBUMSSTYLE_H
#define ALBUMSSTYLE_H

#include <mwidgetstyle.h>

class M_EXPORT AlbumsStyle : public MWidgetStyle
{
    Q_OBJECT
    M_STYLE(AlbumsStyle)
};

class M_EXPORT AlbumsStyleContainer : public MWidgetStyleContainer
{
    M_STYLE_CONTAINER(AlbumsStyle)
};

#endif // ALBUMSSTYLE_H
