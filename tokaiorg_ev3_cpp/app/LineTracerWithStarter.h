/******************************************************************************
 *  LineTracerWithStarter.h (for LEGO Mindstorms EV3)
 *  Created on: 2015/01/26
 *  Implementation of the Class LineTracerWithStarter
 *  Author: Kazuhiro Kawachi
 *  Copyright (c) 2015 Embedded Technology Software Design Robot Contest
 *****************************************************************************/

#ifndef EV3_APP_LINETRACERWITHSTARTER_H_
#define EV3_APP_LINETRACERWITHSTARTER_H_

#include "Starter.h"
#include "LineTracer.h"
#include "Calibration.h"

class LineTracerWithStarter {
public:
    LineTracerWithStarter(LineTracer* lineTracer,
               const Starter* starter,
               Calibration* calibration);
    virtual ~LineTracerWithStarter();

    void run();

private:
    enum State {
        UNDEFINED,
        CALIBRATION_GYRO,
        CALIBRATION_BLACK,
        CALIBRATION_WHITE,
        WAITING_FOR_START,
        WALKING
    };

    LineTracer* mLineTracer;
    const Starter* mStarter;
    Calibration* mCalibration;
    State mState;

    void execUndefined();
    void execCalibrationGyro();
    void execCalibrationBlack();
    void execCalibrationWhite();
    void execWaitingForStart();
    void execWalking();
};

#endif  // EV3_APP_LINETRACERWITHSTARTER_H_
