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

//#ifdef DEBUG
//#define _debug(x) (x)
//#else
//#define _debug(x)
//#endif
//
//Clock* myFileClock;
//
//
//void file_task(intptr_t unused) {
//   // myFileClock->getTim()
//
//    char firstLine[] = "システムクロック時刻,ジャイロ角速度,左PWM,右PWM,電圧";
//    myFileClock = new Clock();
//    volatile int iTime;
//    //iTime = Clock.myFileClock->getTim();
//    //FILE* fpLog;
//    //
//    //if ((fpLog = fopen("log.csv", "w")) != NULL){
//    //    ev3_led_set_color(LED_RED);
//    //    //fwrite(firstLine, sizeof(firstLine), 1, fpLog);
//    //    //fwrite(iTime , sizeof(int), 1, fpLog);
//
//
//
//    //    //fprintf(fpLog,"%s",firstLine);
//    //    while (1){
//    //        //ev3_led_set_color(LED_RED);
//    //        //fprintf(fpLog, "%d,%d", myFileClock->getTim(), myFileGyro->getAnglerVelocity());
//
//    //    //fwrite(firstLine, sizeof(firstLine), 1, fpLog);
//    //        myFileClock->sleep(100);
//
//    //    }
//    //    fclose(fpLog);
//
//    //}
//    
//
//}


