/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */


#include "mcontentitemex.h"

void MContentItemEx::setPixmap(const QPixmap &pixmap)
{
    MContentItem::setPixmap(pixmap);
}

void MContentItemEx::showSpinner()
{
    MProgressIndicator *org = dynamic_cast<MProgressIndicator *>(additionalItem());
    if (!org) {
        MProgressIndicator *spinner = new MProgressIndicator(this, MProgressIndicator::spinnerType);
        spinner->setUnknownDuration(true);
        spinner->setObjectName("loadingSpinner");
        setAdditionalItem(spinner);
        delete org;
    }
}

void MContentItemEx::hideSpinner()
{
    MProgressIndicator *org = dynamic_cast<MProgressIndicator *>(additionalItem());
    setAdditionalItem(NULL);
    if (org)
        delete org;
}

