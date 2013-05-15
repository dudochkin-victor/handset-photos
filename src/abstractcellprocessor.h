/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */


#ifndef ABSTRACTCELLPROCESSOR_H
#define ABSTRACTCELLPROCESSOR_H
#include <MContentItem>
class AbstractCellProcessor
{
public:
    virtual void processCell(MContentItem *cell) = 0;
};

#endif // ABSTRACTCELLPROCESSOR_H
