#include "ev3api.h"
#include "stdlib.h"
#include "struct.h"

namespace ev3api {


	class queueClass
	{
	public:
		queueClass();
		~queueClass();
		void enqueue(int,int,signed char,signed char,int);	//�l���i�[����
		int dequeue(LogQueData*);	//�l���o�͂���

	private:
		Que* que;
		LogQueData* logQueData;
	};
}
