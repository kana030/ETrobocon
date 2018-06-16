#include "ev3api.h"
#include "stdlib.h"
#include "struct.h"

namespace ev3api{


    class fileWriteClass
    {
    public:
        fileWriteClass();
        ~fileWriteClass();
		void logFileWrite(FILE*,LogQueData*);
    };
}
