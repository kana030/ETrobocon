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

#define STRATE_SPEED (15)   //���i�̍ۂ̃X�s�[�h
#define TURN_LEFT (-30)     //���܂���ۂ̒l
#define TURN_RIGHT (30)     //�E�܂���ۂ̒l

Motor* myLeftMotor = new Motor(PORT_B);                 //���[�^��
Motor* myRightMotor = new Motor(PORT_A);                //���[�^�E
GyroSensor* myGyroSensor = new GyroSensor(PORT_4);      //�W���C���Z���T
Clock* myClock = new Clock();                           //����
ColorSensor* myColorSensor = new ColorSensor(PORT_2);   //�J���[�Z���T
TouchSensor* myTouchSensor = new TouchSensor(PORT_1);   //�^�b�`�Z���T

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
* ���C���^�X�N
*/
void main_task(intptr_t unused)
{
  bool flag_start = false;    //�o�����X�R���g���[�����J�n������

  ev3_led_set_color(LED_OFF);
  //�L�����u���[�V����
  getColor(&color_white);
  getColor(&color_black);

  while (1)
  {
    if (myTouchSensor->isPressed())
    {
      if (flag_start == false)     //���s�J�n�O�̏ꍇ
      {
        myClock->reset();
        act_tsk(FILE_TASK);
        //�o�����X�R���g���[���̊J�n
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
* 4msec���Ƃɍs���^�X�N
* �o�����X�R���g���[���A�J���[�Z���T�̒l
*/
void linetrace_task(intptr_t idx){

  //���O�̒l�̕ϐ�
  int iTime;
  int iAnglerVelocity;
  signed char retLeftPWM;
  signed char retRightPWM;
  int iBatteryVoltage;

  //�J���[�Z���T�̎擾
  int judgeColor = (color_black + color_white) / 2;

  if (myColorSensor->getBrightness() < judgeColor)         //���̏ꍇ
  {
    turnSpeed = TURN_RIGHT;
  }
  else   //���̏ꍇ
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
* �L�����u���[�V�������s���֐�
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
* �t�@�C���������݂��s���^�X�N
*/
void file_task(intptr_t unused)
{

  LogQueData* fileWriteQue;
  fileWriteQue = (struct LogQueData *)malloc(sizeof(struct LogQueData));

  //���O�̒l�̕ϐ�
  FILE *fpLog;
  fpLog = fopen("log.csv", "a");
  myClock->now();
  if (fpLog == NULL)
  {
    return;
  }
  else
  {
    fprintf(fpLog, "�V�X�e���N���b�N����,�W���C���p���x,��PWM,�EPWM,�d��\n");
    while (1){
      if(pqueueClass->dequeue(fileWriteQue) == 0){
        pfileWriteClass->logFileWrite(fpLog,fileWriteQue);
      }
    }
  }
  fclose(fpLog);
  free(fileWriteQue);
}
