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

#define QUEUE_SIZE 200      //最大データ数

typedef struct {            //キューのための構造体
    int head;               
    int num;
    int time[QUEUE_SIZE];
    int anglerVelocity[QUEUE_SIZE];
    signed char retRightPWM[QUEUE_SIZE];
    signed char retLeftPWM[QUEUE_SIZE];
    int batteryVoltage[QUEUE_SIZE];

} queue_t;

Motor* myRightMotor;    //モータ右
Motor* myLeftMotor;     //モータ左
GyroSensor* myGyro;     //ジャイロセンサ
Clock* myClock;         //クロック

queue_t que;

void main_task(intptr_t unused) {

    myRightMotor = new Motor(PORT_A);
    myLeftMotor = new Motor(PORT_B);
    myGyro = new GyroSensor(PORT_4);
    myClock = new Clock();
    myClock->reset();
    que.num = 0;
    que.head = 0;

    //バランスコントロールの開始
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

        que.time[que.num] = myClock->now();         //キューの最大サイズの場合0に戻る
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
        fprintf(fpLog, "システムクロック時刻,ジャイロ角速度,左PWM,右PWM,電圧\n");
        while (1){

            //if (que.time[(que.head + que.num) % QUEUE_SIZE] > 0){
            //int i = que.num;
            //for (int j = i; j > 0; j--){
            //    iNum = (que.head + i) % QUEUE_SIZE;
            fprintf(fpLog, "%d,%d,%d,%d,%d,\n",
                que.time[que.head],             //システムクロック時刻
                que.anglerVelocity[que.head],   //ジャイロ角速度
                que.retLeftPWM[que.head],       //左PWM
                que.retRightPWM[que.head],      //右PWM
                que.batteryVoltage[que.head]);  //電圧
            que.head++;

            if (que.head == QUEUE_SIZE){        //キューの最大サイズの場合0に戻る
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
