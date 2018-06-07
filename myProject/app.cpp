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
#include "PIDcontrol.h"

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

#define STRATE_SPEED (70)   //直進の際のスピード

#define PWM_ABS_MAX (80)

Motor* myLeftMotor = new Motor(PORT_B);                 //モータ左
Motor* myRightMotor = new Motor(PORT_A);                //モータ右
Motor* myTailMotor = new Motor(PORT_C);                 //尻尾モータ
GyroSensor* myGyroSensor = new GyroSensor(PORT_4);      //ジャイロセンサ
Clock* myClock = new Clock();                           //時間
ColorSensor* myColorSensor = new ColorSensor(PORT_2);   //カラーセンサ
TouchSensor* myTouchSensor = new TouchSensor(PORT_1);   //タッチセンサ

void getColor(int*);
int checkColor(rgb_raw_t*);
void file_task(intptr_t);
void linetrace_cyc(intptr_t);
static void tail_control(int);

float pid_control(int,int);

int color_black = 0;  //黒
int color_white = 0;  //白
int judgeColor = 0; //灰色
int getGyroOffset = 0;  //ジャイロセンサオフセット

rgb_raw_t rgb_val;

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

  myLeftMotor->setPWM(0);
  myRightMotor->setPWM(0);
  balance_init();     //初期化
  myLeftMotor->reset();
  myRightMotor->reset();
  judgeColor = (color_black + color_white) / 2;

  while (1)
  {
    if (myTouchSensor->isPressed())   //タッチセンサが押されたとき
    {
      if (flag_start)         //走行終わり
      {
        ev3_led_set_color(LED_ORANGE);
        ev3_stp_cyc(LINETRACE_CYC);
        myLeftMotor->setPWM(0);
        myRightMotor->setPWM(0);
        myClock->sleep(500);  //fflushのため
        break;
      }
      else    //走行開始
      {
        //getGyroOffset = myTailMotor->getCount()+180;
        //getGyroOffset = (2/3) * ( myTailMotor->getCount() + 70);
        //myGyroSensor->setOffset(getGyroOffset);
        tail_control(0);
        act_tsk(FILE_TASK);       //ファイルタスク開始
        myClock->reset();
        //バランスコントロールの開始
        ev3_sta_cyc(LINETRACE_CYC);
        flag_start = true;
        myClock->sleep(500);
        ev3_led_set_color(LED_OFF);
      }
    }
    else    //タッチセンサが押されていないとき
    {
      if(flag_start)
      {
        myClock->sleep(4);
      }
      else
      {
        tail_control(90);
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
void linetrace_cyc(intptr_t idx)
{
  //ログの値の変数
  int iAnglerVelocity;
  signed char retLeftPWM;
  signed char retRightPWM;
  int iBatteryVoltage;
  //カラーセンサの取得
  myColorSensor->getRawColor(rgb_val);
  iAnglerVelocity = myGyroSensor->getAnglerVelocity();
  iBatteryVoltage = ev3_battery_voltage_mV();

  balance_control(
    (float)STRATE_SPEED,
    (float)pid_control(checkColor(&rgb_val),judgeColor),
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
  //pqueueClass->enqueue(iTime,iAnglerVelocity,retLeftPWM,retRightPWM,iBatteryVoltage);
  pqueueClass->enqueue(myClock->now(),iAnglerVelocity,retLeftPWM,retRightPWM,iBatteryVoltage,checkColor(&rgb_val));
}

/*
* キャリブレーションを行う関数
*/
void getColor(int* color){
  while (1){
    if (myTouchSensor->isPressed()){
      myColorSensor->getRawColor(rgb_val);
      *color = checkColor(&rgb_val);
      ev3_led_set_color(LED_GREEN);
      myClock->sleep(500);
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
  if (fileWriteQue == NULL)
   {
    ev3_led_set_color(LED_RED);		//mallocで領域が確保できなかった場合
  }

  //ログの値の変数
  FILE *fpLog;
  fpLog = fopen("log_wakana.csv", "a");
  if (fpLog == NULL)
  {
    return;
  }
  else
  {
    //fprintf(fpLog, "システムクロック時刻,ジャイロ角速度,左PWM,右PWM,電圧\n");
    fprintf(fpLog, "システムクロック時刻,ジャイロ角速度,左PWM,右PWM,電圧,カラーセンサの値,白%d,黒%d\n",color_white,color_black);
    while (1)
    {
      if(pqueueClass->dequeue(fileWriteQue) == 0)
      {
        pfileWriteClass->logFileWrite(fpLog,fileWriteQue);
      }else
      {
        fflush(fpLog);	//書き出す
      }
    }
  }
  fclose(fpLog);
  free(fileWriteQue);
}

static void tail_control(int angle)
{
  float pwm = (float)( (int)angle - myTailMotor->getCount() ); /* 比例制御 */
  int Kp = 1;
  pwm = pwm * Kp;
  /* PWM出力飽和処理 */
  if (pwm > PWM_ABS_MAX)
  {
    pwm = PWM_ABS_MAX;
  }
  else if (pwm < -PWM_ABS_MAX)
  {
    pwm = -PWM_ABS_MAX;
  }

  if (pwm == 0)
  {
    myTailMotor->setPWM(0);
  }
  else
  {
    myTailMotor->setPWM((signed char)pwm);
  }
}

int checkColor(rgb_raw_t *rgb_get_color){

  return (rgb_get_color->r+rgb_get_color->g+rgb_get_color->b)/3;
}
