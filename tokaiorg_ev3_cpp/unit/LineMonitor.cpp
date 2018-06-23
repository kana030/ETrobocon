/******************************************************************************
 *  LineMonitor.cpp (for LEGO Mindstorms EV3)
 *  Created on: 2015/01/25
 *  Definition of the Class LineMonitor
 *  Author: Kazuhiro Kawachi
 *  Copyright (c) 2015 Embedded Technology Software Design Robot Contest
 *****************************************************************************/


#include "LineMonitor.h"

// �萔�錾
const int8_t LineMonitor::INITIAL_THRESHOLD = 20;  // ���F�̌��Z���T�l

/**
 * �R���X�g���N�^
 * @param colorSensor �J���[�Z���T
 */
LineMonitor::LineMonitor(const ev3api::ColorSensor& colorSensor)
    : mColorSensor(colorSensor),
      mLineThreshold(INITIAL_THRESHOLD),
      mWhiteThresh(INITIAL_THRESHOLD),
      mBlackThresh(INITIAL_THRESHOLD) {
}

/**
 * �f�X�g���N�^
 */
LineMonitor::~LineMonitor() {
}

/**
 * ���C���ォ�ۂ��𔻒肷��
 * @retval true  ���C����
 * @retval false ���C���O
 */
bool LineMonitor::isOnLine() const {
    // ���Z���T����̎擾�l������
    // ���ȏ�ł���΁utrue�v���A
    // �����łȂ���΁ufalse�v��Ԃ�
    if (mColorSensor.getBrightness() >= mLineThreshold) {
        return true;
    } else {
        return false;
    }
}

/**
 * ���C��臒l��ݒ肷��B
 * @param threshold ���C��臒l
 */
void LineMonitor::setLineThreshold(int8_t threshold) {
    mLineThreshold = threshold;
}

/**
 * ���C��臒l����̕΍����擾����
 */
int8_t LineMonitor::getDeviation() const {
    return (mLineThreshold - mColorSensor.getBrightness());
}


/**
 * ���C��臒l���Z�o���Đݒ肷��
 */
void LineMonitor::calLineThreshold() {
    int16_t  cal;

    cal = (mBlackThresh + mWhiteThresh) / 2;
    setLineThreshold(cal);
}


/**
 * ���F��臒l��ݒ肷��
 */
void LineMonitor::setBlackThreshold(int8_t threshold) {
    mBlackThresh = threshold;
}


/**
 * ���F��臒l��ݒ肷��
 */
void LineMonitor::setWhiteThreshold(int8_t threshold) {
    mWhiteThresh = threshold;
}
