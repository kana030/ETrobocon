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
#include "stdlib.h"

#include "Motor.h"
#include "GyroSensor.h"
#include "Clock.h"
#include "ColorSensor.h"
#include "TouchSensor.h"

using namespace ev3api;

#define DEBUG

#ifdef DEBUG
#define _debug(x) (x)
#else
#define _debug(x)
#endif

#define QUEUE_SIZE (200)    //�ő�f�[�^��
#define STRATE_SPEED (15)   //���i�̍ۂ̃X�s�[�h
#define TURN_LEFT (-30)     //���܂���ۂ̒l
#define TURN_RIGHT (30)     //�E�܂���ۂ̒l
#define COLOR_BLACK (10)    //�J���[�Z���T�̍��̒l
#define COLOR_WHITE (30)   //�J���[�Z���T�̔��̒l

typedef struct QueData
{
    int time;
    int anglerVelocity;
    signed char retLeftPWM;
    signed char retRightPWM;
    int batteryVoltage;
} QueData;

typedef struct Que
{
    int head;
    int num;
    struct QueData qyeArray[QUEUE_SIZE];
} Que;

Motor* myLeftMotor;             //���[�^��
Motor* myRightMotor;            //���[�^�E
GyroSensor* myGyroSensor;       //�W���C���Z���T
Clock* myClock;                 //����
ColorSensor* myColorSensor;     //�J���[�Z���T

//���O�̒l�̕ϐ�
int iAnglerVelocity;
signed char retLeftPWM;
signed char retRightPWM;
int iRightMotor;
int iLeftMotor;
int iBatteryVoltage;

void queue_reset(Que*);
void enqueue(Que*, int, int, signed char, signed char, int);
void dequeue(Que*, FILE*);
void queue_clear(Que*);
void file_task(intptr_t);


Que que;

float straightSpeed = 10;
float turnSpeed = 0;

/*
* ���C���^�X�N
*/
void main_task(intptr_t unused)
{
    myLeftMotor = new Motor(PORT_B);
    myRightMotor = new Motor(PORT_A);
    myGyroSensor = new GyroSensor(PORT_4);
    myClock = new Clock();
    myClock->reset();
    queue_reset(&que);
    act_tsk(FILE_TASK);

    //�o�����X�R���g���[���̊J�n
    while (1)
    {
        iAnglerVelocity = myGyroSensor->getAnglerVelocity();
        iBatteryVoltage = ev3_battery_voltage_mV();

        balance_control(
            (float)straightSpeed,
            (float)turnSpeed,
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
    queue_clear(&que);
}

/*
* �t�@�C���������݂��s���^�X�N
*/
void file_task(intptr_t unused)
{
    FILE* fpLog;
    fpLog = fopen("log.csv", "a");
    if (fpLog == NULL)
    {
        return;
    }
    else
    {
        fprintf(fpLog, "�V�X�e���N���b�N����,�W���C���p���x,��PWM,�EPWM,�d��\n");
        while (1){
            dequeue(&que, fpLog);
            myClock->sleep(4);
        }
    }
    fclose(fpLog);
}

/*
* �J���[�Z���T�̒l�̓ǂݎ����s���^�X�N
*/
void sensor_task(intptr_t unused)
{
    myColorSensor = new ColorSensor(PORT_2);

    while (1)
    {
        if (myColorSensor->getBrightness() < COLOR_BLACK)         //���̏ꍇ
        {
            turnSpeed = TURN_RIGHT;
        }
        else if (myColorSensor->getBrightness() > COLOR_WHITE)   //���̏ꍇ
        {
            turnSpeed = TURN_LEFT;
        }
        else
        {
            turnSpeed = 0;  //���i
        }
        myClock->sleep(4);
    }
}


/*
* �L���[�̏�����
*/
void queue_reset(Que *que)
{
    que->head = 0;
    que->num = 0;
    que = (Que *)malloc(sizeof(Que) * QUEUE_SIZE);
    que->qyeArray[que->num].time = 0;
    que->qyeArray[que->num].anglerVelocity = 0;
    que->qyeArray[que->num].retLeftPWM = 0;
    que->qyeArray[que->num].retRightPWM = 0;
    que->qyeArray[que->num].batteryVoltage = 0;

}

/*
* �L���[�ɒl����
*/
void enqueue(Que *que, int time, int anglerVelocity, signed char retLeftPWM, signed char retRightPWM, int batteryVoltage)
{
    if (que->num < QUEUE_SIZE) {
        int iNum = (que->head + que->num) % QUEUE_SIZE;
        que->qyeArray[iNum].time = time;
        que->qyeArray[iNum].anglerVelocity = anglerVelocity;
        que->qyeArray[iNum].retLeftPWM = retLeftPWM;
        que->qyeArray[iNum].retRightPWM = retRightPWM;
        que->qyeArray[iNum].batteryVoltage = batteryVoltage;
        que->num++;
    }
}

/*
* �L���[�̒l���t�@�C���ɏ����o��
*/
void dequeue(Que *que, FILE* fpLog)
{
    if (que->num > 0) {
        fprintf(
            fpLog, "%d,%d,%d,%d,%d\n",
            que->qyeArray[que->head].time,               //�V�X�e���N���b�N����
            que->qyeArray[que->head].anglerVelocity,     //�W���C���p���x
            que->qyeArray[que->head].retLeftPWM,         //��PWM
            que->qyeArray[que->head].retRightPWM,        //�EPWM
            que->qyeArray[que->head].batteryVoltage      //�d��
            );
        que->head = (que->head + 1) % QUEUE_SIZE;
        que->num--;
    }
}

void queue_clear(Que *que){
    free(que);
}
