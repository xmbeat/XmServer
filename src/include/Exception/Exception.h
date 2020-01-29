/*
 * Exception.h
 *
 *  Created on: 08/07/2012
 *      Author: xmbeat
 */

#ifndef EXCEPTION_H
#define EXCEPTION_H
#include "../String/String.h"
class Exception {
	public:
		Exception(String message):
			_message(message){}
		virtual String getMessage()
		{
			return _message;
		}
		virtual ~Exception(){}
	private:
		String _message;
};

#endif /* EXCEPTION_H_ */
