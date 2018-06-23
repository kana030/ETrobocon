/******************************************************************************
 *  app.cpp (for LEGO Mindstorms EV3)
 *  Created on: 2015/01/25
 *  Implementation of the Task main_task
 *  Author: Kazuhiro.Kawachi
 *  Copyright (c) 2015 Embedded Technology Software Design Robot Contest
 *****************************************************************************/

#include "app.h"
#include "LineTracerWithStarter.h"

#if defined(BUILD_MODULE)
#include "module_cfg.h"
#else
#include "kernel_cfg.h"
#endif

// using�錾
using ev3api::ColorSensor;
using ev3api::GyroSensor;
using ev3api::TouchSensor;
using ev3api::Motor;

// Device objects
// �I�u�W�F�N�g��ÓI�Ɋm�ۂ���
ColorSensor gColorSensor(PORT_3);
GyroSensor  gGyroSensor(PORT_4);
TouchSensor gTouchSensor(PORT_1);
Motor       gLeftWheel(PORT_C);
Motor       gRightWheel(PORT_B);

// �I�u�W�F�N�g�̒�`
static LineMonitor     *gLineMonitor;
static Balancer        *gBalancer;
static BalancingWalker *gBalancingWalker;
static LineTracer      *gLineTracer;
static Starter         *gStarter;
static Calibration     *gCalibration;
static PidController   *gPidController;
static LineTracerWithStarter *gLineTracerWithStarter;

/**
 * EV3�V�X�e������
 */
static void user_system_create() {
	tslp_tsk(2);
    // �I�u�W�F�N�g�̍쐬
    gBalancer        = new Balancer();
    gBalancingWalker = new BalancingWalker(gGyroSensor,
                                           gLeftWheel,
                                           gRightWheel,
                                           gBalancer);
    gLineMonitor     = new LineMonitor(gColorSensor);
    gStarter         = new Starter(gTouchSensor);
    gPidController   = new PidController();
    gLineTracer      = new LineTracer(gLineMonitor, gBalancingWalker, gPidController);
    gCalibration     = new Calibration(gColorSensor, gGyroSensor, gLineMonitor);
    gLineTracerWithStarter = new LineTracerWithStarter(gLineTracer, gStarter, gCalibration);

    // �����������ʒm
    ev3_led_set_color(LED_ORANGE);
}

/**
 * EV3�V�X�e���j��
 */
static void user_system_destroy() {
    gLeftWheel.reset();
    gRightWheel.reset();

    delete gLineTracerWithStarter;
    delete gLineTracer;
    delete gStarter;
    delete gLineMonitor;
    delete gBalancingWalker;
    delete gBalancer;
    delete gCalibration;
    delete gPidController;
}

/**
 * �g���[�X���s�^�C�~���O
 */
void ev3_cyc_tracer(intptr_t exinf) {
    act_tsk(TRACER_TASK);
}

/**
 * ���C���^�X�N
 */
void main_task(intptr_t unused) {
    user_system_create();  // �Z���T�⃂�[�^�̏���������

    // �����n���h���J�n
    ev3_sta_cyc(EV3_CYC_TRACER);

    slp_tsk();  // �o�b�N�{�^�����������܂ő҂�

    // �����n���h����~
    ev3_stp_cyc(EV3_CYC_TRACER);

    user_system_destroy();  // �I������

    ext_tsk();
}

/**
 * ���C���g���[�X�^�X�N
 */
void tracer_task(intptr_t exinf) {
    if (ev3_button_is_pressed(BACK_BUTTON)) {
        wup_tsk(MAIN_TASK);  // �o�b�N�{�^������
    } else {
        gLineTracerWithStarter->run();  // �|�����s
    }

    ext_tsk();
}
