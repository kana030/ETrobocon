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
	logQueData = (struct LogQueData *)malloc(sizeof(struct LogQueData));
	que->head = 0;
	que->num = 0;
}

queueClass::~queueClass()
{
	free(que);
	free(logQueData);
}

/*�l���i�[*/
void queueClass::enqueue(int time,int iAnglerVelocity,signed char retLeftPWM,signed char retRightPWM,int iBatteryVoltage) {
	if ((que->num ) < QUEUE_SIZE) {
		int iNum = (que->num + que->head ) % QUEUE_SIZE;
		que->queArray[iNum] = {time,iAnglerVelocity,retLeftPWM,retRightPWM,iBatteryVoltage};
		que->num++;

	}
	else    //�l���i�[�ł��Ȃ��ꍇ
	{
		ev3_led_set_color(LED_RED);
	}
}

/*
*�l���o��
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
