/******************************************************************************
 *  PidController.cpp (for LEGO Mindstorms EV3)
 *  Created on: 2015/06/12
 *  Implementation of the Class BalancingWalker
 *  Copyright (c) 2015 Embedded Technology Software Design Robot Contest
 *****************************************************************************/

#ifndef EV3_UNIT_PIDCONTROLLER_H_
#define EV3_UNIT_PIDCONTROLLER_H_

#include "ev3api.h"


class PidController
{
public:
    PidController();
    virtual ~PidController();

    /**
     * PIDêßå‰Ç≈TurnílÇéZèoÇ∑ÇÈ
     */
    int8_t calControlledVariable(int8_t deviation);

private:
    int32_t mDeviation;
    int32_t mIntegral;
};

#endif  // EV3_UNIT_PIDCONTROLLER_H_
