/**
 * This sample program balances a two-wheeled Segway type robot such as Gyroboy in EV3 core set.
 *
 * References:
 * http://www.hitechnic.com/blog/gyro-sensor/htway/
 * http://www.cs.bgu.ac.il/~ami/teaching/Lejos-2013/classes/src/lejos/robotics/navigation/Segoway.java
 */

#include "ev3api.h"
#include "app.h"
#include "balancer.h"
#include "libcpp-test.h"

#include "Motor.h"
#include "GyroSensor.h"
#include "Clock.h"

using namespace ev3api;

#define DEBUG

#ifdef DEBUG
#define _debug(x) (x)
#else
#define _debug(x)
#endif

Motor* myRightMotor;    //���[�^�E
Motor* myLeftMotor;     //���[�^��
GyroSensor* myGyro;     //�W���C���Z���T
signed char retRightPWM;
signed char retLeftPWM;
Clock* myClock;

//���O�̒l�p
int iRightMotor;
int iLeftMotor;
int iAnglerVelocity;
int iTime;


void main_task(intptr_t unused) {

    myRightMotor = new Motor(PORT_A);
    myLeftMotor = new Motor(PORT_B);
    myGyro = new GyroSensor(PORT_4);
    myClock = new Clock();
    myClock->reset();

    //�X�^�[�g
    while (1)
    {
        //iAnglerVelocity = myGyro->getAnglerVelocity();
        balance_control(
            (float)0,
            (float)0,
            (float)myGyro->getAnglerVelocity(),
            (float)0,
            (float)myLeftMotor->getCount(),
            (float)myRightMotor->getCount(),
            (float)ev3_battery_voltage_mV(),
            &retLeftPWM,
            &retRightPWM
            );
        myRightMotor->setPWM(retRightPWM);
        myLeftMotor->setPWM(retLeftPWM);
        myClock->sleep(4);
    }
}

void file_task(intptr_t unused) {

    //char firstLine[] = "�V�X�e���N���b�N����,�W���C���p���x,��PWM,�EPWM,�d��\n";
    FILE* fpLog;
    fpLog = fopen("log.csv", "a");
    if (fpLog == NULL){
        return;
    }


    //fprintf(fpLog, "%s\n", firstLine);
    fprintf(fpLog, "�V�X�e���N���b�N����,�W���C���p���x,��PWM,�EPWM,�d��\n");
    while (1){
        fprintf(fpLog, "%d,�W���C���p���x,��PWM,�EPWM,�d��\n", myClock->now());
        myClock->sleep(4);

    }
    //    ev3_led_set_color(LED_RED);
    //    myClock->sleep(4);
    fclose(fpLog);

}
