/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */


#ifndef PHOTOSSTYLE_H
#define PHOTOSSTYLE_H

#include <mwidgetstyle.h>

class M_EXPORT PhotosStyle : public MWidgetStyle
{
    Q_OBJECT
    M_STYLE(PhotosStyle)
};

class M_EXPORT PhotosStyleContainer : public MWidgetStyleContainer
{
    M_STYLE_CONTAINER(PhotosStyle)
};

#endif // PHOTOSSTYLE_H
