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

Motor* myRightMotor;    //モータ右
Motor* myLeftMotor;     //モータ左
GyroSensor* myGyro;     //ジャイロセンサ
signed char retRightPWM;
signed char retLeftPWM;
Clock* myClock;

//ログの値用
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

    //スタート
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

    //char firstLine[] = "システムクロック時刻,ジャイロ角速度,左PWM,右PWM,電圧\n";
    FILE* fpLog;
    fpLog = fopen("log.csv", "a");
    if (fpLog == NULL){
        return;
    }


    //fprintf(fpLog, "%s\n", firstLine);
    fprintf(fpLog, "システムクロック時刻,ジャイロ角速度,左PWM,右PWM,電圧\n");
    while (1){
        fprintf(fpLog, "%d,ジャイロ角速度,左PWM,右PWM,電圧\n", myClock->now());
        myClock->sleep(4);

    }
    //    ev3_led_set_color(LED_RED);
    //    myClock->sleep(4);
    fclose(fpLog);

}
