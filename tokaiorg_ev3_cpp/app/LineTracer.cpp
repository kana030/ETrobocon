/******************************************************************************
 *  LineTracer.cpp (for LEGO Mindstorms EV3)
 *  Created on: 2015/01/26
 *  Implementation of the Class LineTracer
 *  Author: Kazuhiro Kawachi
 *  Copyright (c) 2015 Embedded Technology Software Design Robot Contest
 *****************************************************************************/

#include "LineTracer.h"

/**
 * コンストラクタ
 * @param lineMonitor     ライン判定
 * @param balancingWalker 倒立走行
 */
LineTracer::LineTracer(const LineMonitor* lineMonitor,
                       BalancingWalker* balancingWalker,
                       PidController* pidController)
    : mLineMonitor(lineMonitor),
      mBalancingWalker(balancingWalker),
      mPidController(pidController),
      mIsInitialized(false) {
}

/**
 * デストラクタ
 */
LineTracer::~LineTracer() {
}

/**
 * ライントレースする
 */
void LineTracer::run() {
    if (mIsInitialized == false) {
        mBalancingWalker->init();
        mIsInitialized = true;
    }

    bool isOnLine = mLineMonitor->isOnLine();

    // 走行体の向きを計算する
    int direction = calcDirection(isOnLine);

    mBalancingWalker->setCommand(BalancingWalker::LOW, direction);

    // 倒立走行を行う
    mBalancingWalker->run();
}

/**
 * 走行体の向きを計算する
 * @param isOnLine true:ライン上/false:ライン外
 * @retval 30  ライン上にある場合(右旋回指示)
 * @retval -30 ライン外にある場合(左旋回指示)
 */
int LineTracer::calcDirection(bool isOnLine) {

// ★PID演習で有効にする。
    return mPidController->calControlledVariable(mLineMonitor->getDeviation());


// ★PID演習でここから無効にする。
/*
    if (isOnLine) {
        return BalancingWalker::LOW;
    } else {
        return -BalancingWalker::LOW;
    }
    */
// ★PID演習でここまで無効にする。
}
