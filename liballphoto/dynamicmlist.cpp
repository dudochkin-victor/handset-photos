/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */


#include "dynamicmlist.h"
#include "mcontentitemcreator.h"


DynamicMList::DynamicMList(MWidget *parent, MContentItem::ContentItemStyle itemStyle, int id) :
    MList(parent), tasklet(NULL)
{
    MContentItemCreator *thumbnailItemCreator = new MContentItemCreator(itemStyle, id);
    setCellCreator(thumbnailItemCreator);
}

DynamicMList::~DynamicMList()
{
   if (tasklet)
       disconnect(this, SIGNAL(panningStarted()), tasklet, SLOT(stopJobQueue()));

   if (tasklet)
        delete tasklet;

}

void DynamicMList::setTasklet (AbstractTasklet *newTasklet)
{
    if (!newTasklet)
        return;

    if (tasklet) {
        disconnect(this, SIGNAL(panningStarted()), tasklet, SLOT(stopJobQueue()));
        delete tasklet;
    }

    tasklet = newTasklet;
    connect(this, SIGNAL(panningStarted()), tasklet, SLOT(stopJobQueue()));
}
