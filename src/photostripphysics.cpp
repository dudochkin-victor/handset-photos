/*
 * Meego-handset-photos is a photo viewer.
 * Copyright (C) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */


#include "photostripphysics.h"
#include <QtGui>

static const int PositionNoiseDampingDelta    =       2; /* in px */


PhotoStripPhysics::PhotoStripPhysics(QObject *parent) :
    MPhysics2DPanning(parent)
{
}

PhotoStripPhysics::~PhotoStripPhysics()
{
}

void PhotoStripPhysics::integrateAxis(
            Qt::Orientation orientation, 
            qreal &position, 
            qreal &velocity,
            qreal &acceleration,
            qreal &pointerDifference,
            bool pointerPressed)
{
    qreal force;
    qreal rangeStart = (orientation == Qt::Vertical ? range().top() : range().left());
    qreal rangeEnd =   (orientation == Qt::Vertical ? range().bottom() : range().right());

    // Damping
    if (position >= rangeStart && position <= rangeEnd) {

        // Inside range
        if (pointerPressed) {
            force = -friction() * velocity;
        } else {
            force = -slidingFriction() * velocity;
        }
    } else {
        // Outside range (in border)
        force = -borderFriction() * velocity;
    }

    // Border springs
    if (position < rangeStart) {
        force += borderSpringK() * (rangeStart - position);
        //Special case - when the border is crossed,
        //we don't want the view to "bounce" from the border.
        //We snap in this place to rangeStart value;
        if ((pointerPressed == false) && (position + velocity + force >= rangeStart)) {
            velocity = force = 0;
            position = rangeStart;
        }
    }

    if (position > rangeEnd) {
        force += -borderSpringK() * (position - rangeEnd);
        //Special case - when the border is crossed
        //we don't want the view to "bounce" from the border.
        //We snap in this place to rangeEnd value;
        if ((pointerPressed == false) && (position + velocity + force <= rangeEnd)) {
            velocity = force = 0;
            position = rangeEnd;
        }
    }

    // Integration. Currently does not use time_delta or mass (assumed as 1.0)

    if (pointerPressed) {

        // Damping of acceleration with pointer spring values.
        force += pointerSpringK() * pointerDifference;
        // Increasing the speed by the last movement of the pointer
        acceleration = force - pointerDifference;
        velocity += acceleration;

        position           -= pointerDifference;
        pointerDifference = 0;

    } else {
        // The window that we should be snapping
        int snapTarget = (int)((position / (qreal)snapInterval_) + 0.5);
        force += borderSpringK() * (snapTarget * (qreal)snapInterval_ - position);
        if (snapTarget != currentIndex && snapTarget >= 0)
        {
            currentIndex = snapTarget;
            emit snapIndexChanged(currentIndex);
        }


        acceleration = force - pointerDifference;

        velocity                 += acceleration;
        position                 += velocity;
        pointerDifference        = 0;
    }
}
void PhotoStripPhysics::setSnapInterval(uint newSnapInterval)
{
    snapInterval_ = newSnapInterval;
    qDebug() << "PhotoStripPhysics::setSnapInterval:" << snapInterval_;
}

uint PhotoStripPhysics::snapInterval()
{

    return snapInterval_;
}
