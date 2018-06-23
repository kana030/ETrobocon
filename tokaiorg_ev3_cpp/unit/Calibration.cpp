/******************************************************************************
 *  Calibration.cpp (for LEGO Mindstorms EV3)
 *  Created on: 2015/06/12
 *  Implementation of the Class BalancingWalker
 *  Copyright (c) 2015 Embedded Technology Software Design Robot Contest
 *****************************************************************************/

#include "Calibration.h"


// �萔�錾
const int Calibration::NUM_OF_GYRO_CALIBRATION =  250; // ���L�����u���[�V�������K�C���ӏ�
const int Calibration::NUM_OF_BLACK_CALIBRATION = 250; // ���L�����u���[�V�������K�C���ӏ�
const int Calibration::NUM_OF_WHITE_CALIBRATION = 250; // ���L�����u���[�V�������K�C���ӏ�
// const int Calibration::NUM_OF_GYRO_CALIBRATION =  1; // ���L�����u���[�V�������K�C���ӏ�
// const int Calibration::NUM_OF_BLACK_CALIBRATION = 1; // ���L�����u���[�V�������K�C���ӏ�
// const int Calibration::NUM_OF_WHITE_CALIBRATION = 1; // ���L�����u���[�V�������K�C���ӏ�

/**
 * �R���X�g���N�^
 * @param colorSensor �J���[�Z���T
 * @param gyroSensor  �W���C���Z���T
 * @param lineMonitor ���C�����j�^
 */
Calibration::Calibration(const ev3api::ColorSensor& colorSensor,
                  ev3api::GyroSensor& gyroSensor,
                  LineMonitor* lineMonitor)
    : mColorSensor(colorSensor),
      mGyroSensor(gyroSensor),
      mLineMonitor(lineMonitor),
      mIsStartedGyro(false),
      mIsStartedBlack(false),
      mIsStartedWhite(false),
      mCalCount(0),
      mSum(0) {
}

/**
 * �f�X�g���N�^
 */
Calibration::~Calibration() {
}


/**
 * �L�����u���[�V�����ɕK�v�Ȃ��̂����Z�b�g����
 */
void Calibration::init() {

    mGyroSensor.setOffset(0);                           // �W���C���Z���T�I�t�Z�b�g������
}

/**
 * �W���C���Z���T�̃I�t�Z�b�g�l���L�����u���[�V��������
 * ���߂�l��
 *    false: �L�����u���[�V����������
 *    true : �L�����u���[�V��������
 */
bool Calibration::calibrateGyro(bool startTrigger) {

    int16_t  sensor;
    int16_t  cal;
    bool finish;
    char buf[256];

    finish = false;
    sensor = mGyroSensor.getAnglerVelocity();

    if (mIsStartedGyro == false) {

        sprintf( buf, "gyro = %03d", sensor);           // �W���C���Z���T�l��\��
        ev3_lcd_draw_string( buf, 0, 10);

        if (startTrigger == true) {

            mIsStartedGyro = true;
            mSum = 0;
            mCalCount = 0;
        }
    }
    else {

        mSum += sensor;                                 // �W���C���Z���T�l��ώZ
        mCalCount++;

        if (mCalCount == NUM_OF_GYRO_CALIBRATION) {     // �K��񐔈ȏ�ώZ

          cal = (mSum / mCalCount);
            //cal = (1);                                  // ���ϒl ���L�����u���[�V�������K�C���ӏ�
            mGyroSensor.setOffset(cal);

            sprintf( buf, "gyroOffset = %03d", cal);    // �W���C���I�t�Z�b�g�l��\��
            ev3_lcd_draw_string( buf, 0, 10);

            finish = true;                              // ����
        }
    }
    return finish;
}

/**
 * ���F��臒l���L�����u���[�V��������
 * ���߂�l��
 *    false: �L�����u���[�V����������
 *    true : �L�����u���[�V��������
 */
bool Calibration::calibrateBlack(bool startTrigger) {

    int8_t  sensor;
    int16_t  cal;
    bool finish;
    char buf[256];

    finish = false;
    sensor = mColorSensor.getBrightness();

    if (mIsStartedBlack == false) {

        sprintf( buf, "black = %03d", sensor);          // ���Z���T�l��\��
        ev3_lcd_draw_string( buf, 0, 20);

        if (startTrigger == true) {

            mIsStartedBlack = true;
            mSum = 0;
            mCalCount = 0;
        }
    }
    else {

        mSum += sensor;                                 // ���Z���T�l��ώZ
        mCalCount++;

        if (mCalCount == NUM_OF_BLACK_CALIBRATION) {    // �K��񐔈ȏ�ώZ

            cal = (mSum / mCalCount);                                  // ���ϒl ���L�����u���[�V�������K�C���ӏ�
            //cal = (1);                                  // ���ϒl ���L�����u���[�V�������K�C���ӏ�
            mLineMonitor->setBlackThreshold(cal);

            sprintf( buf, "blackTh = %03d", cal);       // ���������l��\��
            ev3_lcd_draw_string( buf, 0, 20);

            finish = true;                              // ����
        }
    }
    return finish;
}

/**
 * ���F��臒l���L�����u���[�V��������
 * ���߂�l��
 *    false: �L�����u���[�V����������
 *    true : �L�����u���[�V��������
 */
bool Calibration::calibrateWhite(bool startTrigger) {

    int8_t  sensor;
    int16_t  cal;
    bool finish;
    char buf[256];

    finish = false;
    sensor = mColorSensor.getBrightness();

    if (mIsStartedWhite == false) {

        sprintf( buf, "white = %03d", sensor);          // ���Z���T�l��\��
        ev3_lcd_draw_string( buf, 0, 30);

        if (startTrigger == true) {

            mIsStartedWhite = true;
            mSum = 0;
            mCalCount = 0;
        }
    }
    else {

        mSum += sensor;                                 // ���Z���T�l��ώZ
        mCalCount++;

        if (mCalCount == NUM_OF_WHITE_CALIBRATION) {    // �K��񐔈ȏ�ώZ

            cal = (mSum / mCalCount);                                  // ���ϒl ���L�����u���[�V�������K�C���ӏ�
            cal = (1);                                  // ���ϒl ���L�����u���[�V�������K�C���ӏ�
            mLineMonitor->setWhiteThreshold(cal);

            sprintf( buf, "whiteTh = %03d", cal);       // ���������l��\��
            ev3_lcd_draw_string( buf, 0, 30);

            finish = true;                              // ����
        }
    }
    return finish;
}

/**
 * ���C���g���[�X�������l��ݒ�
 */
void Calibration::calibrateLineThreshold() {
    mLineMonitor->calLineThreshold();
}
