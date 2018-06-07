#include "PIDcontrol.h"

float pid_control(int sensor_val,int target_val){
  float p, i,d;
  static short diff[2];
  static float integral;
  diff[0] = diff[1];
  diff[1] = sensor_val - target_val;
  integral += (diff[1] + diff[0]) / 2.0 * DELTA_T;

  p = KP * diff[1];
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
