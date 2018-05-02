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
    signed char retLeftPWM[QUEUE_SIZE];
    signed char retRightPWM[QUEUE_SIZE];
    int batteryVoltage[QUEUE_SIZE];

} queue_t;

Motor* myLeftMotor;     //���[�^��
Motor* myRightMotor;    //���[�^�E
GyroSensor* myGyro;     //�W���C���Z���T
Clock* myClock;         //����

//���O�̒l�p
int iAnglerVelocity;
signed char retLeftPWM;
signed char retRightPWM;
int iRightMotor;
int iLeftMotor;
int iBatteryVoltage;

void enqueue(queue_t*, int, int, signed char, signed char, int);
queue_t que;

void main_task(intptr_t unused) {

    myLeftMotor = new Motor(PORT_B);
    myRightMotor = new Motor(PORT_A);
    myGyro = new GyroSensor(PORT_4);
    myClock = new Clock();
    myClock->reset();
    que.num = 0;
    que.head = 0;

    //�o�����X�R���g���[���̊J�n
    while (1)
    {
        iAnglerVelocity = myGyro->getAnglerVelocity();
        iBatteryVoltage = ev3_battery_voltage_mV();

        balance_control(
            (float)0,
            (float)0,
            (float)iAnglerVelocity,
            (float)0,
            (float)myLeftMotor->getCount(),
            (float)myRightMotor->getCount(),
            (float)iBatteryVoltage,
            &retLeftPWM,
            &retRightPWM
            ); 
        myLeftMotor->setPWM(retLeftPWM);
        myRightMotor->setPWM(retRightPWM);
        enqueue(&que, myClock->now(), iAnglerVelocity, retLeftPWM, retRightPWM, iBatteryVoltage);
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

            myClock->sleep(4);

        }
    }

    fclose(fpLog);

}

void enqueue(queue_t *que, int time, int anglerVelocity, signed char retLeftPWM,signed char retRightPWM, int batteryVoltage)
{

    que->time[que->num] = time;
    que->anglerVelocity[que->num] = anglerVelocity;
    que->retLeftPWM[que->num] = retLeftPWM;
    que->retRightPWM[que->num] = retRightPWM;
    que->batteryVoltage[que->num] = batteryVoltage;
    que->num++;


    if (que->num == QUEUE_SIZE){            //�L���[�̍ő�T�C�Y�̏ꍇ0�ɖ߂�      
        que->num = 0;
    }


}
