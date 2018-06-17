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
		void enqueue(uint32_t,int,signed char,signed char,int);	//エンキュー
		void enqueue(unsigned int,int,signed char,signed char,int,int);	//エンキュー カラーセンサ追加
		int dequeue(LogQueData*);														//デキュー

	private:
		Que* que;
		LogQueData* logQueData;
	};
}
