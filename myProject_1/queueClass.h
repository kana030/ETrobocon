#pragma once
#include "ev3api.h"
#include "stdlib.h"
#include "struct.h"

namespace ev3api {


	class queueClass
	{
	public:
		queueClass();
		~queueClass();
		void enqueue(uint32_t,int,signed char,signed char,int);	//�G���L���[
		void enqueue(unsigned int,int,signed char,signed char,int,int);	//�G���L���[ �J���[�Z���T�ǉ�
		int dequeue(LogQueData*);														//�f�L���[

	private:
		Que* que;
		LogQueData* logQueData;
	};
}