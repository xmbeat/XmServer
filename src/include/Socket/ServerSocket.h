#ifndef SERVERSOCKET_H
#define SERVERSOCKET_H
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "Socket.h"
class ServerSocket
{
    public:
        ServerSocket(int);
        virtual Socket accept();
        virtual void close();
        virtual ~ServerSocket() {}
    private:
        int _port;
        int _socketDescriptor;
};

ServerSocket::ServerSocket(int port):_port(port)
{
    struct sockaddr_in servAddr;

    _socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (_socketDescriptor == -1)
        throw SocketException("No se pudo abrir el socket");
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = INADDR_ANY;
    servAddr.sin_port = htons(_port);
    if (bind(_socketDescriptor, (struct sockaddr*) &servAddr, sizeof(servAddr))==-1)
        throw SocketException("Excepcion enlazando, verifica que el puerto este disponible");
    if (listen(_socketDescriptor, 10) == -1)
        throw SocketException("Excepcion poniendo en escucha");
}

Socket ServerSocket::accept()
{
    struct sockaddr_in clientAddr;
    socklen_t clientLen = sizeof(clientAddr);
    int newSocketDescriptor = ::accept(_socketDescriptor,(struct sockaddr*)
                                     &clientAddr, &clientLen);
    if (newSocketDescriptor == -1)
        throw SocketException("Excepcion en accept()");

    return Socket(newSocketDescriptor);
}

void ServerSocket::close()
{
    if (_socketDescriptor != -1)
        ::close(_socketDescriptor);
}
#endif // SERVERSOCKET_H
