#ifndef XMCLIENTSESSION_H
#define XMCLIENTSESSION_H
#include "Socket/Socket.h"
#include "String/String.h"
#include "Exception/Exception.h"
#include "SessionUtils.h"
#include <iostream>
using namespace std;
using namespace SessionUtils;
class XmClientSession
{
	private:
		Socket* _socket;
		String _host;
		int _port;
	public:
		XmClientSession():
			_socket(NULL),
			_host("localhost"),
			_port(15000)
		{}
		XmClientSession(String host, int port):
			_host(host),
			_port(port)
		{}
		virtual void stop()
		{
			if (_socket && _socket->isOpen())
			{
				MetadataList header;
				header.add("QUIT",0);
				sendPackage(&header, NULL, _socket);
				_socket->close();
			}
		}
		virtual void connect()
		{
			_socket = new Socket(_host, _port);
		}
		virtual bool getFile(String srcFile, String destFile, XProgressFunc fun = NULL)
		{
			try
			{
				MetadataList header;
				header.add("GET", srcFile);
				header.add("BYTE-POS", 0);
				sendPackage(&header, NULL, _socket);
				header = extractMetadata(_socket);
				for (int i = 0; i < header.size(); i++)
				{
					std::cout << header[i].toString() << std::endl;
				}
				if (header[0].getValue().equals("GOOD", true))
				{
					HeadAttrib *attrib = header.getAttribByName("CONTENT-LENGTH");
					int len = attrib->getIntValue();
					saveResource(_socket, len, destFile, fun);
					std::cout << "Recurso guardado!\n";
					delete attrib;
					return true;
				}
			}
			catch(Exception &e)
			{
				std::cout << "getFile error: " << e.getMessage() << std::endl;
			}
			catch(...){
				std::cout << "excepcion no reconocida\n";
			}
			return false;
		}
		virtual ~XmClientSession()
		{
			if (_socket) delete _socket;
		}
};
int main(int c, char**args)
{
	XmClientSession sesion;
	sesion.connect();
	sesion.getFile(args[1], args[2]);
	sesion.stop();
}
#endif

