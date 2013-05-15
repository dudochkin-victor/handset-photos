/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef STRIPIMAGESTYLE_H
#define STRIPIMAGESTYLE_H

#include <mimagewidgetstyle.h>

class StripImageStyle : public MImageWidgetStyle
{
    Q_OBJECT
    M_STYLE(StripImageStyle)
};

class StripImageStyleContainer : public MImageWidgetStyleContainer
{
    M_STYLE_CONTAINER(StripImageStyle)
    M_STYLE_MODE(Small)
};


#endif // STRIPIMAGESTYLE_H
