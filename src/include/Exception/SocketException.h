#ifndef SOCKETEXCEPTION_H
#define SOCKETEXCEPTION_H
#include "Exception.h"
class SocketException: public Exception
{
    public:
        SocketException(String message): Exception(message){}
};
#endif // SOCKETEXCEPTION_H
