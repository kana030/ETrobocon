#include "ev3api.h"
#include "app.h"
#include "balancer.h"
#include "libcpp-test.h"
#include "stdlib.h"

#include "lineTrace.h"

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
#define COLOR_WHITE (30)    //�J���[�Z���T�̔��̒l

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

lineTrace::lineTrace()
{
}

lineTrace::~lineTrace()
{
}

balancerRun
/*
* ���C���^�X�N
*/
void main_task(intptr_t unused)
{
    myLeftMotor = new Motor(PORT_B);
    myRightMotor = new Motor(PORT_A);
    myColorSensor = new ColorSensor(PORT_2);
    myGyroSensor = new GyroSensor(PORT_4);
    myClock = new Clock();

    //�J���[�Z���T�̎擾
    int judgeColor = (COLOR_BLACK + COLOR_WHITE) / 2;
    if (myColorSensor->getBrightness() < judgeColor)         //���̏ꍇ
    {
        turnSpeed = TURN_RIGHT;
    }
    else   //���̏ꍇ
    {
        turnSpeed = TURN_LEFT;
    }
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

