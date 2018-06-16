#pragma once
#define QUEUE_SIZE (200)    //キューの大きさ

struct LogQueData
{
	uint32_t time;
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
