#include "ev3api.h"
#include "libcpp-test.h"
#include "stdlib.h"

#include "fileWriteClass.h"
#include "queueClass.h"

using namespace ev3api;

fileWriteClass::fileWriteClass()
{
}

fileWriteClass::~fileWriteClass()
{
	//free();
}
/*
* ���O�o�͂��s��
* �����F�����o���t�@�C���̃t�@�C���|�C���^�A�����o���\����
*/
void fileWriteClass::logFileWrite(FILE *fpLog,LogQueData *logQueData) {
	fprintf(fpLog,"%lu,%d,%d,%d,%d\n",
	logQueData->time,
	logQueData->anglerVelocity,
	logQueData->retLeftPWM,
	logQueData->retRightPWM,
	logQueData->batteryVoltage
);
}
