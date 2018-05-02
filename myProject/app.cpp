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
#include "ColorSensor.h"

using namespace ev3api;

#define DEBUG

#ifdef DEBUG
#define _debug(x) (x)
#else
#define _debug(x)
#endif

#define QUEUE_SIZE 200      //�ő�f�[�^��

typedef struct {            //�L���[�̍\����
    int head;
    int tail;
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
ColorSensor* myColor;   //�J���[�Z���T


//���O�̒l�p
int iAnglerVelocity;
signed char retLeftPWM;
signed char retRightPWM;
int iRightMotor;
int iLeftMotor;
int iBatteryVoltage;

void enqueue(queue_t*, int, int, signed char, signed char, int);
void dequeue(queue_t*, FILE*);

queue_t que;
/*
* ���C���^�X�N
*/
void main_task(intptr_t unused) {

    myLeftMotor = new Motor(PORT_B);
    myRightMotor = new Motor(PORT_A);
    myGyro = new GyroSensor(PORT_4);
    myClock = new Clock();
    myColor = new ColorSensor(PORT_2);

    myClock->reset();
    que.tail = 0;
    que.head = 0;

    //�o�����X�R���g���[���̊J�n
    while (1)
    {
        iAnglerVelocity = myGyro->getAnglerVelocity();
        iBatteryVoltage = ev3_battery_voltage_mV();

        balance_control(
            (float)15,
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


/*
* �t�@�C���������݂��s���^�X�N
*/
void file_task(intptr_t unused) {

    FILE* fpLog;
    //int iNum;
    fpLog = fopen("log.csv", "a");
    if (fpLog == NULL){
        return;
    }
    else
    {
        dequeue(&que,fpLog);
        fprintf(fpLog, "�V�X�e���N���b�N����,�W���C���p���x,��PWM,�EPWM,�d��\n");
        while (1){

            dequeue(&que, fpLog);
            myClock->sleep(4);

        }
    }

    fclose(fpLog);

}

void enqueue(queue_t *que, int time, int anglerVelocity, signed char retLeftPWM, signed char retRightPWM, int batteryVoltage)
{

    que->time[que->tail] = time;
    que->anglerVelocity[que->tail] = anglerVelocity;
    que->retLeftPWM[que->tail] = retLeftPWM;
    que->retRightPWM[que->tail] = retRightPWM;
    que->batteryVoltage[que->tail] = batteryVoltage;
    que->tail++;


    if (que->tail == QUEUE_SIZE){            //�L���[�̍ő�T�C�Y�̏ꍇ0�ɖ߂�      
        que->tail = 0;
    }


}

void dequeue(queue_t *que, FILE* fpLog)
{

    fprintf(fpLog, "%d,%d,%d,%d,%d,\n",
        que->time[que->head],             //�V�X�e���N���b�N����
        que->anglerVelocity[que->head],   //�W���C���p���x
        que->retLeftPWM[que->head],       //��PWM
        que->retRightPWM[que->head],      //�EPWM
        que->batteryVoltage[que->head]);  //�d��
    que->head++;

    if (que->head == QUEUE_SIZE){        //�L���[�̍ő�T�C�Y�̏ꍇ0�ɖ߂�
        que->head = 0;
    }


}

