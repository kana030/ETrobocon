#pragma once
#define QUEUE_SIZE (200)    //�ő��f�[�^��

struct LogQueData
{
	int time;
	int anglerVelocity;
	signed char retLeftPWM;
	signed char retRightPWM;
	int batteryVoltage;
};

struct Que
{
	int head;
	int num;
	struct LogQueData queArray[QUEUE_SIZE];
};
