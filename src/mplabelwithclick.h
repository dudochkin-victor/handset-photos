/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */


#ifndef MPLABELWITHCLICK_H
#define MPLABELWITHCLICK_H

#include <MLabel>
#include <QModelIndex>

class MPLabelWithClick : public MLabel
{
    Q_OBJECT
public:
    MPLabelWithClick(QGraphicsItem *parent = 0, MLabelModel *model = 0) : MLabel(parent, model) {};

    explicit MPLabelWithClick(QString const &text, QGraphicsItem *parent = 0): MLabel(text, parent) {};
    virtual ~MPLabelWithClick() {};

protected:
     virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {Q_UNUSED(event); emit clicked();};
     virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) {Q_UNUSED(event);};

Q_SIGNALS:
     void clicked();

};

#endif // MPLABELWITHCLICK_H
