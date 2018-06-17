#include "ev3api.h"
#include "libcpp-test.h"
#include "stdlib.h"

#include "queueClass.h"

using namespace ev3api;

#define DEBUG

#ifdef DEBUG
#define _debug(x) (x)
#else
#define _debug(x)
#endif


queueClass::queueClass()
{
	que = (struct Que *)malloc(sizeof(struct Que) * QUEUE_SIZE);
	if (que == NULL) {
		ev3_led_set_color(LED_RED);		//mallocで領域が確保できなかった場合
	}
	logQueData = (struct LogQueData *)malloc(sizeof(struct LogQueData));
	if (logQueData == NULL) {
		ev3_led_set_color(LED_RED);		//mallocで領域が確保できなかった場合
	}
	que->head = 0;
	que->num = 0;
}

queueClass::~queueClass()
{
	free(que);
	free(logQueData);
}

/*
* エンキュー
* 引数 : 保存する時間、角速度、左PWM、右PWM、バッテリー残量 戻り値 : 無し
*/
void queueClass::enqueue(uint32_t time,int iAnglerVelocity,signed char retLeftPWM,signed char retRightPWM,int iBatteryVoltage) {
	if ((que->num ) < QUEUE_SIZE) {
		int iNum = (que->num + que->head ) % QUEUE_SIZE;
		que->queArray[iNum] = {time,iAnglerVelocity,retLeftPWM,retRightPWM,iBatteryVoltage};
		que->num++;
		ev3_led_set_color(LED_OFF);
	}
	else    //200件(キューの大きさ)を超えて保存しようとした場合
	{
		ev3_led_set_color(LED_RED);
	}
}

/*
* エンキュー
* 引数 : 保存する時間、角速度、左PWM、右PWM、バッテリー残量、カラーセンサ取得値 戻り値 : 無し
*/
void queueClass::enqueue(uint32_t time,int iAnglerVelocity,signed char retLeftPWM,signed char retRightPWM,int iBatteryVoltage,int iColor) {
	if ((que->num ) < QUEUE_SIZE) {
		int iNum = (que->num + que->head ) % QUEUE_SIZE;
		que->queArray[iNum] = {time,iAnglerVelocity,retLeftPWM,retRightPWM,iBatteryVoltage,iColor};
		que->num++;
		ev3_led_set_color(LED_OFF);
	}
	else    //200件(キューの大きさ)を超えて保存しようとした場合
	{
		ev3_led_set_color(LED_RED);
	}
}

/*
* デキュー
* 引数 : ファイル出力する構造体 戻り値 : 0...書き出す値があるとき 1...書き出す値がないとき
*/
int queueClass::dequeue(LogQueData *outputQue) {
	if (que->num > 0) {
		*outputQue = que->queArray[que->head];
		que->head = ((que->head) + 1) % QUEUE_SIZE;
		que->num--;
		return 0;
	}
	return -1;
}
