/******************************************************************************
 *  BalancerCpp.cpp (for LEGO Mindstorms EV3)
 *  Created on: 2015/01/25
 *  Implementation of the Class Balancer
 *  Author: Kazuhiro.Kawachi
 *  Copyright (c) 2015 Embedded Technology Software Design Robot Contest
 *****************************************************************************/

#include "balancer.h"

#include "BalancerCpp.h"

/**
 * �R���X�g���N�^
 */
Balancer::Balancer()
    : mForward(0),
      mTurn(0),
      mOffset(0),
      mRightPwm(0),
      mLeftPwm(0) {
}

/**
 * �f�X�g���N�^
 */
Balancer::~Balancer() {
}

/**
 * �o�����T������������
 * @param offset �W���C���Z���T�I�t�Z�b�g�l
 */
void Balancer::init(int offset) {
    mOffset = offset;
    balance_init();  // �|���U�q���䏉����
}

/**
 * �o�b�N���b�V���L�����Z��
 * @note          ���߂�PWM�l�ɉ����ăG���R�[�_�l�Ƀo�b�N���b�V�����̒l��ǉ����܂�
 * @param rwEnc   �E�ԗփG���R�[�_�l
 * @param lwEnc   ���ԗփG���R�[�_�l
 * @date          2017/10/24
 * @auther        Koji SHIMIZU
 */
void Balancer::cancelBacklash(int& rwEnc, int& lwEnc)
{
    const int BACKLASHHALF = 4;   // �o�b�N���b�V���̔���[deg]

    if(mRightPwm < 0) rwEnc += BACKLASHHALF;
    else if(mRightPwm > 0) rwEnc -= BACKLASHHALF;

    if(mLeftPwm < 0) lwEnc += BACKLASHHALF;
    else if(mLeftPwm > 0) lwEnc -= BACKLASHHALF;
}

/**
 * �o�����T�̒l���X�V����
 * @param angle   �p���x
 * @param rwEnc   �E�ԗփG���R�[�_�l
 * @param lwEnc   ���ԗփG���R�[�_�l
 * @param battety �o�b�e���d���l
 */
void Balancer::update(int angle, int rwEnc, int lwEnc, int battery) {
    cancelBacklash(rwEnc, lwEnc); // �o�b�N���b�V���L�����Z��
    // �|���U�q����API���Ăяo���A�|�����s���邽�߂�
    // ���E���[�^�o�͒l�𓾂�
    balance_control(
        static_cast<float>(mForward),
        static_cast<float>(mTurn),
        static_cast<float>(angle),
        static_cast<float>(mOffset),
        static_cast<float>(lwEnc),
        static_cast<float>(rwEnc),
        static_cast<float>(battery),
        &mLeftPwm,
        &mRightPwm);
}

/**
 * PWM�l��ݒ肷��
 * @param forward �O�i�l
 * @param turn    ����l
 */
void Balancer::setCommand(int forward, int turn) {
    mForward = forward;
    mTurn    = turn;
}

/**
 * �E�ԗւ�PWM�l���擾����
 * @return �E�ԗւ�PWM�l
 */
int8_t Balancer::getPwmRight() {
    return mRightPwm;
}

/**
 * ���ԗւ�PWM�l���擾����
 * @return ���ԗւ�PWM�l
 */
int8_t Balancer::getPwmLeft() {
    return mLeftPwm;
}
