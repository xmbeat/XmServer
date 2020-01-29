#include "Exception.h"
class SecurityException: public Exception
{
public:
	SecurityException(String msg):Exception(msg){}
};
