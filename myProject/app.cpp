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

#include "struct.h"

#include "queueClass.h"
#include "fileWriteClass.h"

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

#define STRATE_SPEED (15)   //直進の際のスピード
#define TURN_LEFT (-30)     //左折する際の値
#define TURN_RIGHT (30)     //右折する際の値

Motor* myLeftMotor = new Motor(PORT_B);                 //モータ左
Motor* myRightMotor = new Motor(PORT_A);                //モータ右
GyroSensor* myGyroSensor = new GyroSensor(PORT_4);      //ジャイロセンサ
Clock* myClock = new Clock();                           //時間
ColorSensor* myColorSensor = new ColorSensor(PORT_2);   //カラーセンサ
TouchSensor* myTouchSensor = new TouchSensor(PORT_1);   //タッチセンサ

void getColor(int*);
void file_task(intptr_t);
void linetrace_task(intptr_t);

int color_black = 0;
int color_white = 0;
float straightSpeed = 100;
float turnSpeed = 10;

queueClass *pqueueClass = new queueClass();
fileWriteClass *pfileWriteClass = new fileWriteClass();

/*
* メインタスク
*/
void main_task(intptr_t unused)
{
  bool flag_start = false;    //バランスコントロールが開始したか

  ev3_led_set_color(LED_OFF);
  //キャリブレーション
  getColor(&color_white);
  getColor(&color_black);

  while (1)
  {
    if (myTouchSensor->isPressed())
    {
      if (flag_start == false)     //走行開始前の場合
      {
        myClock->reset();
        act_tsk(FILE_TASK);
        //バランスコントロールの開始
        ev3_sta_cyc(LINETRACE_TASK);
        flag_start = true;
        myClock->wait(1000);
        ev3_led_set_color(LED_OFF);
      }
      else
      {
        ev3_led_set_color(LED_ORANGE);
        ev3_stp_cyc(LINETRACE_TASK);
        myLeftMotor->setPWM(0);
        myRightMotor->setPWM(0);
        break;
      }
    }
  }
  delete pqueueClass;
  delete pfileWriteClass;
}

/*
* 4msecごとに行うタスク
* バランスコントロール、カラーセンサの値
*/
void linetrace_task(intptr_t idx){

  //ログの値の変数
  int iTime;
  int iAnglerVelocity;
  signed char retLeftPWM;
  signed char retRightPWM;
  int iBatteryVoltage;

  //カラーセンサの取得
  int judgeColor = (color_black + color_white) / 2;

  if (myColorSensor->getBrightness() < judgeColor)         //黒の場合
  {
    turnSpeed = TURN_RIGHT;
  }
  else   //白の場合
  {
    turnSpeed = TURN_LEFT;
  }

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
iTime = myClock->now();
  myLeftMotor->setPWM(retLeftPWM);
  myRightMotor->setPWM(retRightPWM);
   pqueueClass->enqueue(iTime,iAnglerVelocity,retLeftPWM,retRightPWM,iBatteryVoltage);
}

/*
* キャリブレーションを行う関数
*/
void getColor(int* color){
  while (1){
    if (myTouchSensor->isPressed()){
      *color = myColorSensor->getBrightness();
      ev3_led_set_color(LED_GREEN);
      myClock->wait(1000);
      break;
    }
  }
  ev3_led_set_color(LED_OFF);
}

/*
* ファイル書き込みを行うタスク
*/
void file_task(intptr_t unused)
{

  LogQueData* fileWriteQue;
  fileWriteQue = (struct LogQueData *)malloc(sizeof(struct LogQueData));

  //ログの値の変数
  FILE *fpLog;
  fpLog = fopen("log.csv", "a");
  myClock->now();
  if (fpLog == NULL)
  {
    return;
  }
  else
  {
    fprintf(fpLog, "システムクロック時刻,ジャイロ角速度,左PWM,右PWM,電圧\n");
    while (1){
      if(pqueueClass->dequeue(fileWriteQue) == 0){
        pfileWriteClass->logFileWrite(fpLog,fileWriteQue);
      }
    }
  }
  fclose(fpLog);
  free(fileWriteQue);
}
