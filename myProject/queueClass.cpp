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
		ev3_led_set_color(LED_RED);		//malloc�ŗ̈悪�m�ۂł��Ȃ������ꍇ
	}
	logQueData = (struct LogQueData *)malloc(sizeof(struct LogQueData));
	if (logQueData == NULL) {
		ev3_led_set_color(LED_RED);		//malloc�ŗ̈悪�m�ۂł��Ȃ������ꍇ
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
* �G���L���[
* ���� : �ۑ����鎞�ԁA�p���x�A��PWM�A�EPWM�A�o�b�e���[�c�� �߂�l : ����
*/
void queueClass::enqueue(uint32_t time,int iAnglerVelocity,signed char retLeftPWM,signed char retRightPWM,int iBatteryVoltage) {
	if ((que->num ) < QUEUE_SIZE) {
		int iNum = (que->num + que->head ) % QUEUE_SIZE;
		que->queArray[iNum] = {time,iAnglerVelocity,retLeftPWM,retRightPWM,iBatteryVoltage};
		que->num++;
		ev3_led_set_color(LED_OFF);
	}
	else    //200��(�L���[�̑傫��)�𒴂��ĕۑ����悤�Ƃ����ꍇ
	{
		ev3_led_set_color(LED_RED);
	}
}

/*
* �G���L���[
* ���� : �ۑ����鎞�ԁA�p���x�A��PWM�A�EPWM�A�o�b�e���[�c�ʁA�J���[�Z���T�擾�l �߂�l : ����
*/
void queueClass::enqueue(uint32_t time,int iAnglerVelocity,signed char retLeftPWM,signed char retRightPWM,int iBatteryVoltage,int iColor) {
	if ((que->num ) < QUEUE_SIZE) {
		int iNum = (que->num + que->head ) % QUEUE_SIZE;
		que->queArray[iNum] = {time,iAnglerVelocity,retLeftPWM,retRightPWM,iBatteryVoltage,iColor};
		que->num++;
		ev3_led_set_color(LED_OFF);
	}
	else    //200��(�L���[�̑傫��)�𒴂��ĕۑ����悤�Ƃ����ꍇ
	{
		ev3_led_set_color(LED_RED);
	}
}

/*
* �f�L���[
* ���� : �t�@�C���o�͂���\���� �߂�l : 0...�����o���l������Ƃ� 1...�����o���l���Ȃ��Ƃ�
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
