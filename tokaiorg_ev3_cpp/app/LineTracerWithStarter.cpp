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
 * コンストラクタ
 * @param lineTracer ライントレーサ
 * @param starter    スタータ
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
 * デストラクタ
 */
LineTracerWithStarter::~LineTracerWithStarter() {
}

/**
 * ライントレースする
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
 * 未定義状態の処理
 */
void LineTracerWithStarter::execUndefined() {

    mCalibration->init();
    mState = CALIBRATION_GYRO;
}

/**
 * ジャイロセンサのキャリブレーション
 */
void LineTracerWithStarter::execCalibrationGyro() {
    if (mCalibration->calibrateGyro(mStarter->isPushed()) == true) {

        mState = CALIBRATION_BLACK;
    }
}

/**
 * 黒キャリブレーション
 */
void LineTracerWithStarter::execCalibrationBlack() {
    if (mCalibration->calibrateBlack(mStarter->isPushed()) == true) {

        mState = CALIBRATION_WHITE;
    }

}

/**
 * 白キャリブレーション
 */
void LineTracerWithStarter::execCalibrationWhite() {
    if (mCalibration->calibrateWhite(mStarter->isPushed()) == true) {

        mState = WAITING_FOR_START;
    }

}

/**
 * 開始待ち状態の処理
 */
void LineTracerWithStarter::execWaitingForStart() {
    if (mStarter->isPushed()) {

        mCalibration->calibrateLineThreshold();

        mState = WALKING;
    }
}

/**
 * 走行中状態の処理
 */
void LineTracerWithStarter::execWalking() {
    mLineTracer->run();
}
