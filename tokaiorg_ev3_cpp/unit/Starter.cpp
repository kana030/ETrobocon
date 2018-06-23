/******************************************************************************
 *  Starter.cpp (for LEGO Mindstorms EV3)
 *  Created on: 2015/01/25
 *  Implementation of the Class Starter
 *  Author: Kazuhiro.Kawachi
 *  Copyright (c) 2015 Embedded Technology Software Design Robot Contest
 *****************************************************************************/

#include "Starter.h"

/**
 * �R���X�g���N�^
 * @param touchSensor �^�b�`�Z���T
 */
Starter::Starter(const ev3api::TouchSensor& touchSensor)
    : mTouchSensor(touchSensor) {
}

/**
 * �f�X�g���N�^
 */
Starter::~Starter() {
}

/**
 * ���������ۂ�
 * @retval true  �������Ă���
 * @retval false �������Ă��Ȃ�
 */
bool Starter::isPushed() const {
    return mTouchSensor.isPressed();
}
