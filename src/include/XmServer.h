#ifndef XMSERVER_H
#define XMSERVER_H
#include "Thread/Thread.h"
#include "Socket/ServerSocket.h"
#include "XmServerSession.h"
class XmServer: public Thread
{
    public:

        XmServer(int port):_server(NULL), _port(port), _isActive(true), ID(0) {}
        virtual void shutdown()
        {
        	_server->close();
            _isActive = false;
            ID = 0;
        }
        virtual void run()
        {
            try
            {
                _server = new ServerSocket(_port);
                while(_isActive)
                {
                    Socket connection = _server->accept();
                    XmServerSession *session = new XmServerSession(connection, true, ID++);
                    session->start();
                }
            }
            catch (SocketException &e)
            {
                std::cout<<e.getMessage() + "\n";
            }
            std::cout<<"server finalizado\n";
        }

        virtual ~XmServer() {
        	delete _server;
        }
    private:
        ServerSocket* _server;
        int _port;
        bool _isActive;
        int ID;
};

#endif // XMSERVER_H
