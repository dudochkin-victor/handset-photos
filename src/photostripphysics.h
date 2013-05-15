/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */


#ifndef PHOTOSTRIPPHYSICS_H
#define PHOTOSTRIPPHYSICS_H

#include <MPhysics2DPanning>

class PhotoStripPhysics : public MPhysics2DPanning
{
    Q_OBJECT

public:
    PhotoStripPhysics(QObject* parent);
    virtual ~PhotoStripPhysics();

    /*!
     * Sets the interval where this strategy will snap the motion.
     */
    void setSnapInterval(uint snapInterval);

    /*!
     * Returns the current snap interval
     */
    uint snapInterval();



protected:
    virtual void integrateAxis(Qt::Orientation orientation, 
                               qreal &position, 
                               qreal &velocity,
                               qreal &acceleration,
                               qreal &pointerDifference,
                               bool pointerPressed);

signals:
    void snapIndexChanged(int index);

private:
    /*! The interval at which the snapping occurs. */
    uint snapInterval_;
    /*! The current index */
    int currentIndex;

};

#endif //PHOTOSTRIPPHYSICS_H
