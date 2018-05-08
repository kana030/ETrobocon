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

#define QUEUE_SIZE (200)    //最大データ数
#define STRATE_SPEED (15)   //直進の際のスピード
#define TURN_LEFT (-30)     //左折する際の値
#define TURN_RIGHT (30)     //右折する際の値
#define COLOR_BLACK (10)    //カラーセンサの黒の値
#define COLOR_WHITE (30)    //カラーセンサの白の値

Motor* myLeftMotor;             //モータ左
Motor* myRightMotor;            //モータ右
GyroSensor* myGyroSensor;       //ジャイロセンサ
Clock* myClock;                 //時間
ColorSensor* myColorSensor;     //カラーセンサ

//ログの値の変数
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
* メインタスク
*/
void main_task(intptr_t unused)
{
    myLeftMotor = new Motor(PORT_B);
    myRightMotor = new Motor(PORT_A);
    myColorSensor = new ColorSensor(PORT_2);
    myGyroSensor = new GyroSensor(PORT_4);
    myClock = new Clock();

    //カラーセンサの取得
    int judgeColor = (COLOR_BLACK + COLOR_WHITE) / 2;
    if (myColorSensor->getBrightness() < judgeColor)         //黒の場合
    {
        turnSpeed = TURN_RIGHT;
    }
    else   //白の場合
    {
        turnSpeed = TURN_LEFT;
    }
    myClock->reset();
    queue_reset(&que);
    act_tsk(FILE_TASK);

    //バランスコントロールの開始
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

