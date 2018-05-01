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

#define QUEUE_SIZE 200      //�ő�f�[�^��

typedef struct {            //�L���[�̂��߂̍\����
    int head;               
    int num;
    int time[QUEUE_SIZE];
    int anglerVelocity[QUEUE_SIZE];
    signed char retRightPWM[QUEUE_SIZE];
    signed char retLeftPWM[QUEUE_SIZE];
    int batteryVoltage[QUEUE_SIZE];

} queue_t;

Motor* myRightMotor;    //���[�^�E
Motor* myLeftMotor;     //���[�^��
GyroSensor* myGyro;     //�W���C���Z���T
Clock* myClock;         //�N���b�N

queue_t que;

void main_task(intptr_t unused) {

    myRightMotor = new Motor(PORT_A);
    myLeftMotor = new Motor(PORT_B);
    myGyro = new GyroSensor(PORT_4);
    myClock = new Clock();
    myClock->reset();
    que.num = 0;
    que.head = 0;

    //�o�����X�R���g���[���̊J�n
    while (1)
    {

        que.anglerVelocity[que.num] = myGyro->getAnglerVelocity();
        que.batteryVoltage[que.num] = ev3_battery_voltage_mV();

        balance_control(
            (float)0,
            (float)0,
            (float)que.anglerVelocity[que.num],
            (float)0,
            (float)myLeftMotor->getCount(),
            (float)myRightMotor->getCount(),
            (float)que.batteryVoltage[que.num],
            &que.retLeftPWM[que.num],
            &que.retRightPWM[que.num]
            );
        myRightMotor->setPWM(que.retRightPWM[que.num]);
        myLeftMotor->setPWM(que.retLeftPWM[que.num]);

        que.time[que.num] = myClock->now();         //�L���[�̍ő�T�C�Y�̏ꍇ0�ɖ߂�
        //int iNum = (que.head + que.num) % QUEUE_SIZE;

        que.num++;

        
        if (que.num == QUEUE_SIZE){         //
            que.num = 0;
        }

        myClock->sleep(4);
    }
}

void file_task(intptr_t unused) {

    FILE* fpLog;
    //int iNum;
    fpLog = fopen("log.csv", "a");
    if (fpLog == NULL){
        return;
    }
    else
    {
        fprintf(fpLog, "�V�X�e���N���b�N����,�W���C���p���x,��PWM,�EPWM,�d��\n");
        while (1){

            //if (que.time[(que.head + que.num) % QUEUE_SIZE] > 0){
            //int i = que.num;
            //for (int j = i; j > 0; j--){
            //    iNum = (que.head + i) % QUEUE_SIZE;
            fprintf(fpLog, "%d,%d,%d,%d,%d,\n",
                que.time[que.head],             //�V�X�e���N���b�N����
                que.anglerVelocity[que.head],   //�W���C���p���x
                que.retLeftPWM[que.head],       //��PWM
                que.retRightPWM[que.head],      //�EPWM
                que.batteryVoltage[que.head]);  //�d��
            que.head++;

            if (que.head == QUEUE_SIZE){        //�L���[�̍ő�T�C�Y�̏ꍇ0�ɖ߂�
                que.head = 0;
            }
            //    que.head = (que.head + 1) % QUEUE_SIZE;
            //}
            //que.num -= i;
            //}


            myClock->sleep(4);

        }
    }

    //    ev3_led_set_color(LED_RED);
    //    myClock->sleep(4);
    fclose(fpLog);

}
