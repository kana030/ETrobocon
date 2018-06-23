/******************************************************************************
 *  LineMonitor.cpp (for LEGO Mindstorms EV3)
 *  Created on: 2015/01/25
 *  Definition of the Class LineMonitor
 *  Author: Kazuhiro Kawachi
 *  Copyright (c) 2015 Embedded Technology Software Design Robot Contest
 *****************************************************************************/


#include "LineMonitor.h"

// 定数宣言
const int8_t LineMonitor::INITIAL_THRESHOLD = 20;  // 黒色の光センサ値

/**
 * コンストラクタ
 * @param colorSensor カラーセンサ
 */
LineMonitor::LineMonitor(const ev3api::ColorSensor& colorSensor)
    : mColorSensor(colorSensor),
      mLineThreshold(INITIAL_THRESHOLD),
      mWhiteThresh(INITIAL_THRESHOLD),
      mBlackThresh(INITIAL_THRESHOLD) {
}

/**
 * デストラクタ
 */
LineMonitor::~LineMonitor() {
}

/**
 * ライン上か否かを判定する
 * @retval true  ライン上
 * @retval false ライン外
 */
bool LineMonitor::isOnLine() const {
    // 光センサからの取得値を見て
    // 黒以上であれば「true」を、
    // そうでなければ「false」を返す
    if (mColorSensor.getBrightness() >= mLineThreshold) {
        return true;
    } else {
        return false;
    }
}

/**
 * ライン閾値を設定する。
 * @param threshold ライン閾値
 */
void LineMonitor::setLineThreshold(int8_t threshold) {
    mLineThreshold = threshold;
}

/**
 * ライン閾値からの偏差を取得する
 */
int8_t LineMonitor::getDeviation() const {
    return (mLineThreshold - mColorSensor.getBrightness());
}


/**
 * ライン閾値を算出して設定する
 */
void LineMonitor::calLineThreshold() {
    int16_t  cal;

    cal = (mBlackThresh + mWhiteThresh) / 2;
    setLineThreshold(cal);
}


/**
 * 黒色の閾値を設定する
 */
void LineMonitor::setBlackThreshold(int8_t threshold) {
    mBlackThresh = threshold;
}


/**
 * 白色の閾値を設定する
 */
void LineMonitor::setWhiteThreshold(int8_t threshold) {
    mWhiteThresh = threshold;
}
