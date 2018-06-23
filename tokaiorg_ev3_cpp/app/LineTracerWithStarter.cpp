/******************************************************************************
 *  LineTracerWithStarter.cpp (for LEGO Mindstorms EV3)
 *  Created on: 2015/01/26
 *  Implementation of the Class LineTracerWithStarter
 *  Author: Kazuhiro Kawachi
 *  Copyright (c) 2015 Embedded Technology Software Design Robot Contest
 *****************************************************************************/

#include "LineTracerWithStarter.h"
#include "ev3api.h"

/**
 * �R���X�g���N�^
 * @param lineTracer ���C���g���[�T
 * @param starter    �X�^�[�^
 */
LineTracerWithStarter::LineTracerWithStarter(LineTracer* lineTracer,
                                             const Starter* starter,
                                             Calibration* calibration)
    : mLineTracer(lineTracer),
      mStarter(starter),
      mCalibration(calibration),
      mState(UNDEFINED){
}

/**
 * �f�X�g���N�^
 */
LineTracerWithStarter::~LineTracerWithStarter() {
}

/**
 * ���C���g���[�X����
 */
void LineTracerWithStarter::run() {
    switch (mState) {
    case UNDEFINED:
        execUndefined();
        break;
    case CALIBRATION_GYRO:
        execCalibrationGyro();
        break;
    case CALIBRATION_BLACK:
        execCalibrationBlack();
        break;
    case CALIBRATION_WHITE:
        execCalibrationWhite();
        break;
    case WAITING_FOR_START:
        execWaitingForStart();
        break;
    case WALKING:
        execWalking();
        break;
    default:
        break;
    }
}

/**
 * ����`��Ԃ̏���
 */
void LineTracerWithStarter::execUndefined() {

    mCalibration->init();
    mState = CALIBRATION_GYRO;
}

/**
 * �W���C���Z���T�̃L�����u���[�V����
 */
void LineTracerWithStarter::execCalibrationGyro() {
    if (mCalibration->calibrateGyro(mStarter->isPushed()) == true) {

        mState = CALIBRATION_BLACK;
    }
}

/**
 * ���L�����u���[�V����
 */
void LineTracerWithStarter::execCalibrationBlack() {
    if (mCalibration->calibrateBlack(mStarter->isPushed()) == true) {

        mState = CALIBRATION_WHITE;
    }

}

/**
 * ���L�����u���[�V����
 */
void LineTracerWithStarter::execCalibrationWhite() {
    if (mCalibration->calibrateWhite(mStarter->isPushed()) == true) {

        mState = WAITING_FOR_START;
    }

}

/**
 * �J�n�҂���Ԃ̏���
 */
void LineTracerWithStarter::execWaitingForStart() {
    if (mStarter->isPushed()) {

        mCalibration->calibrateLineThreshold();

        mState = WALKING;
    }
}

/**
 * ���s����Ԃ̏���
 */
void LineTracerWithStarter::execWalking() {
    mLineTracer->run();
}
