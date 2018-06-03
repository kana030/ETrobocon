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

//PID制御の値
#define DELTA_T 0.004
#define KP 0.7
#define KI 0.0
#define KD 0.0
static short diff[2];
static float integral;

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

#define PWM_ABS_MAX 100

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
void linetrace_task(intptr_t);
float pid_sample(int,int);
static void tail_control(int);

int color_black = 0;
int color_white = 0;
float straightSpeed = 30;
float turnSpeed = 10;
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

  while (1)
  {
    if (myTouchSensor->isPressed())
    {
      if (flag_start == false)     //走行開始前の場合
      {
        myClock->reset();
        act_tsk(FILE_TASK);       //ファイルタスク開始
        //バランスコントロールの開始
        tail_control(0);
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
    else if(flag_start == false &&  !myTouchSensor->isPressed()){
      tail_control(80);
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

  myColorSensor->getRawColor(rgb_val);
  //カラーセンサの取得
  int judgeColor = (color_black + color_white) / 2;

  iAnglerVelocity = myGyroSensor->getAnglerVelocity();
  iBatteryVoltage = ev3_battery_voltage_mV();


    if (checkColor(&rgb_val)< judgeColor)         //黒の場合
    {
      turnSpeed = TURN_LEFT;
    }
    else   //白の場合
    {
      turnSpeed = TURN_RIGHT;
    }

  balance_control(
    (float)straightSpeed,
    (float)turnSpeed,
    //(float)pid_sample(checkColor(&rgb_val),judgeColor),
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
  pqueueClass->enqueue(iTime,iAnglerVelocity,retLeftPWM,retRightPWM,iBatteryVoltage,checkColor(&rgb_val));
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
  if (fileWriteQue == NULL) {
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
    while (1){
      if(pqueueClass->dequeue(fileWriteQue) == 0)
      {
        pfileWriteClass->logFileWrite(fpLog,fileWriteQue);
      }
      else
      {
        fflush(fpLog);	//書き出す
      }
    }
  }
  fclose(fpLog);
  free(fileWriteQue);
}


float pid_sample(int sensor_val,int target_val){

  float p,i,d;
  diff[0] = diff[1];
  diff[1] = sensor_val - target_val;
  integral += (diff[1] + diff[0]) / 2.0 * DELTA_T;

  p= KP*diff[1];
  i = KI * integral;
  d = KD * (diff[1] - diff[0])/DELTA_T;
  if(p+i+d > 100){
    return 100;
  }else if(p+i+d< -100){
    return -100;
  }
  else{
    return p+i+d;
  }
}

static void tail_control(int angle)
{
  float pwm = (float)( (int)angle - myTailMotor->getCount() ); /* 比例制御 */
  pwm = pwm * KP;
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
