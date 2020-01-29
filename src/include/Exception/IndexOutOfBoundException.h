

#ifndef INDEXOUTOFBOUNDEXCEPTION_H
#define INDEXOUTOFBOUNDEXCEPTION_H
#include "Exception.h"

class IndexOutOfBoundException: public Exception {
	public:
		IndexOutOfBoundException(String message):Exception(message){};
		virtual ~IndexOutOfBoundException(){}
	};


#endif
