/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */


#ifndef DYNAMICMLIST_H
#define DYNAMICMLIST_H

#include <QObject>
#include <MAbstractCellCreator>
#include <QAbstractItemModel>
#include <MList>
#include <MContentItem>

#include "abstracttasklet.h"

class AbstractTasklet;

class DynamicMList : public MList
{
Q_OBJECT
public:
    explicit DynamicMList(MWidget *parent = 0, MContentItem::ContentItemStyle style = MContentItem::SingleIcon, int id = 0);
    virtual ~DynamicMList();

    void setCellCreator(MCellCreator * itemCreator)
    {
        creator = itemCreator;
        MList::setCellCreator(itemCreator);
    }

    MCellCreator *cellCreator() const {return creator;}
    void setTasklet (AbstractTasklet *newTasklet);
    AbstractTasklet *getTasklet() const {return tasklet;}
public slots:
    virtual void doTasklet() = 0;

private:
    MCellCreator *creator;
    AbstractTasklet *tasklet;
};

#endif // DYNAMICMLIST_H
