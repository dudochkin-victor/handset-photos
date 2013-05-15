/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef MP_ABSTRACT_LIST_MODEL_OP_H
#define MP_ABSTRACT_LIST_MODEL_OP_H
#include <QModelIndex>
#include <QString>

class MPAbstractListModelOps {
public:
    enum DataType {
        Photo,
        Album
    };

    MPAbstractListModelOps (DataType type) : dataType(type) {};

    DataType getDataType() const {return dataType;};
    virtual void setSpinner(const QModelIndex &index, bool showSpinner) = 0;
    virtual void thumbnailWasLoaded(const QModelIndex &index) = 0;
    virtual void setDefaultThumbnail(int index)  = 0;
    virtual void addOutstandingThumbnailRequest(QString ruquestPath) = 0;
    virtual void removeOutstandingThumbnailRequest(QString ruquestPath) = 0;

    virtual void relayout() = 0;

private:
    DataType dataType;
};
#endif
