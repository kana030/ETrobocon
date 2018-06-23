/******************************************************************************
 *  PidController.cpp (for LEGO Mindstorms EV3)
 *  Created on: 2015/06/12
 *  Implementation of the Class BalancingWalker
 *  Copyright (c) 2015 Embedded Technology Software Design Robot Contest
 *****************************************************************************/

#include "PidController.h"

/**
 * �R���X�g���N�^
 */
PidController::PidController()
    : mDeviation(0),
      mIntegral(0) {
}

/**
 * �f�X�g���N�^
 */
PidController::~PidController() {
}

/**
 * PID����ł�Turn�l���Z�o����
 * @param deviation  ���C����臒l�Ƃ̍����i�΍��j
 */
int8_t PidController::calControlledVariable(int8_t deviation) {
#define KPID_P_FACTOR       (float)( 1.00)
#define KPID_I_FACTOR       (float)( 0.000)
#define KPID_D_FACTOR       (float)( 1.00)
#define KPID_EDGE_FACTOR    (int32_t)(1)        // ���C���g���[�X���� 1 or -1

#define KPID_TURN_LIMIT     (int32_t)(100)      // ����w���l ���E�l

    int32_t brightness_P;                       // P����
    int32_t brightness_I;                       // I����
    int32_t brightness_D;                       // D����
    int32_t turn_P;                             // P�����Z�l
    int32_t turn_I;                             // I�����Z�l
    int32_t turn_D;                             // D�����Z�l
    int32_t turn;                               // ���񖽗�: -100 (������) �` 100 (�E����)

    brightness_P = (int32_t)deviation;
    brightness_I = mIntegral + brightness_P;
    brightness_D = brightness_P - mDeviation;

    mDeviation = brightness_P;                              // ���񉉎Z�p�ɋL��
    mIntegral = brightness_I;

    /* P�����Z */
    turn_P = (int32_t)(KPID_P_FACTOR * (float)brightness_P);
    turn_P *= KPID_EDGE_FACTOR;

    /* I�����Z */
    turn_I = (int32_t)(KPID_I_FACTOR * (float)brightness_I);
    turn_I *= KPID_EDGE_FACTOR;

    /* D�����Z */
    turn_D = (int32_t)(KPID_D_FACTOR * (float)brightness_D);
    turn_D *= KPID_EDGE_FACTOR;

    /* ����w���l�ݒ�   */
    turn = turn_P + turn_I + turn_D;

    if (turn >= KPID_TURN_LIMIT) {
        turn = KPID_TURN_LIMIT;
    } else if (turn <= (-1 * KPID_TURN_LIMIT)) {
        turn = -1 * KPID_TURN_LIMIT;
    }

    return turn;
}
