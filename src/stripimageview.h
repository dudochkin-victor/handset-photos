/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef STRIPIMAGEVIEW_H
#define STRIPIMAGEVIEW_H

#include <mimagewidgetview.h>
#include "stripimagestyle.h"
#include "stripimagemodel.h"
#include <mimagewidget.h>

class StripImageView : public MImageWidgetView
{
    Q_OBJECT
    M_VIEW(StripImageModel, StripImageStyle)

public:
    StripImageView(MImageWidget *controller);
    virtual ~StripImageView();
    void updateStyleMode(int mode);
    void setStyleSmall(bool on);

protected:
    //! \reimp
    virtual void drawContents(QPainter *painter, const QStyleOptionGraphicsItem *option) const;
    //! \reimp_end
private:
    MImageWidget *controller;
};

#endif // STRIPIMAGEVIEW_H
