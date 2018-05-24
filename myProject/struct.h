#pragma once
#define QUEUE_SIZE (200)    //�L���[�̑傫��

struct LogQueData
{
	unsigned int time;
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
