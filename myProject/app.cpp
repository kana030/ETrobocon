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

#define QUEUE_SIZE 200      //最大データ数

typedef struct {            //キューの構造体
    int head;
    int tail;
    int time[QUEUE_SIZE];
    int anglerVelocity[QUEUE_SIZE];
    signed char retLeftPWM[QUEUE_SIZE];
    signed char retRightPWM[QUEUE_SIZE];
    int batteryVoltage[QUEUE_SIZE];

} queue_t;

Motor* myLeftMotor;     //モータ左
Motor* myRightMotor;    //モータ右
GyroSensor* myGyro;     //ジャイロセンサ
Clock* myClock;         //時間
ColorSensor* myColor;   //カラーセンサ


//ログの値用
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
* メインタスク
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

    //バランスコントロールの開始
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
* ファイル書き込みを行うタスク
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
        fprintf(fpLog, "システムクロック時刻,ジャイロ角速度,左PWM,右PWM,電圧\n");
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


    if (que->tail == QUEUE_SIZE){            //キューの最大サイズの場合0に戻る      
        que->tail = 0;
    }


}

void dequeue(queue_t *que, FILE* fpLog)
{

    fprintf(fpLog, "%d,%d,%d,%d,%d,\n",
        que->time[que->head],             //システムクロック時刻
        que->anglerVelocity[que->head],   //ジャイロ角速度
        que->retLeftPWM[que->head],       //左PWM
        que->retRightPWM[que->head],      //右PWM
        que->batteryVoltage[que->head]);  //電圧
    que->head++;

    if (que->head == QUEUE_SIZE){        //キューの最大サイズの場合0に戻る
        que->head = 0;
    }


}

