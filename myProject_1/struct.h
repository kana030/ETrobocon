#pragma once
#define QUEUE_SIZE (200)    //�L���[�̑傫��

struct LogQueData
{
	uint32_t time;
	int anglerVelocity;
	signed char retLeftPWM;
	signed char retRightPWM;
	int batteryVoltage;
	int color;
};

struct Que
{
	int head;
	int num;
	struct LogQueData queArray[QUEUE_SIZE];
};
