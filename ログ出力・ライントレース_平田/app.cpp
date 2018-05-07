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

#define QUEUE_SIZE (200)    //最大データ数
#define STRATE_SPEED (15)   //直進の際のスピード
#define TURN_LEFT (-30)     //左折する際の値
#define TURN_RIGHT (30)     //右折する際の値
#define COLOR_BLACK (10)    //カラーセンサの黒の値
#define COLOR_WHITE (30)   //カラーセンサの白の値

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

/*
* メインタスク
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

/*
* ファイル書き込みを行うタスク
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
        fprintf(fpLog, "システムクロック時刻,ジャイロ角速度,左PWM,右PWM,電圧\n");
        while (1){
            dequeue(&que, fpLog);
            myClock->sleep(4);
        }
    }
    fclose(fpLog);
}

/*
* カラーセンサの値の読み取りを行うタスク
*/
void sensor_task(intptr_t unused)
{
    myColorSensor = new ColorSensor(PORT_2);

    while (1)
    {
        if (myColorSensor->getBrightness() < COLOR_BLACK)         //黒の場合
        {
            turnSpeed = TURN_RIGHT;
        }
        else if (myColorSensor->getBrightness() > COLOR_WHITE)   //白の場合
        {
            turnSpeed = TURN_LEFT;
        }
        else
        {
            turnSpeed = 0;  //直進
        }
        myClock->sleep(4);
    }
}


/*
* キューの初期化
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
* キューに値を代入
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
* キューの値をファイルに書き出す
*/
void dequeue(Que *que, FILE* fpLog)
{
    if (que->num > 0) {
        fprintf(
            fpLog, "%d,%d,%d,%d,%d\n",
            que->qyeArray[que->head].time,               //システムクロック時刻
            que->qyeArray[que->head].anglerVelocity,     //ジャイロ角速度
            que->qyeArray[que->head].retLeftPWM,         //左PWM
            que->qyeArray[que->head].retRightPWM,        //右PWM
            que->qyeArray[que->head].batteryVoltage      //電圧
            );
        que->head = (que->head + 1) % QUEUE_SIZE;
        que->num--;
    }
}

void queue_clear(Que *que){
    free(que);
}
