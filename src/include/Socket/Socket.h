#ifndef SOCKET_H
#define SOCKET_H
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>//para poder usar los servidores DNS y encontrar las IPs de hosts
#include "../Exception/SocketException.h"

class Socket
{
    public:
        Socket();
        Socket(String host, int port){
            connect(host, port);
        }
        Socket(int);
        void close();
        void connect(String, int);
        virtual void write(const char*, int);
        virtual void write(String);
        virtual int read(char*, int);
        virtual String getRemoteHost(){
            return _host;
        }
        virtual bool isOpen(){return _socketDescriptor!=-1;}
        virtual String getLocalHost();
        virtual String getLocalIP();
        virtual int getRemotePort();
        virtual String getRemoteIP();
        virtual ~Socket() {}
    private:
        int _socketDescriptor;
        String _host;
        int _port;
        struct hostent* _server;
        void inline bcopy(char*,char*, int);
        String strIP(unsigned char* dir);
};
//====METODOS IMPLEMENTADOS=====
Socket::Socket(int socketDescriptor)
{
    _socketDescriptor = socketDescriptor;
    _host = getRemoteIP();
    _server = NULL;
}

Socket::Socket()
{
    _host = "localhost";
    _port = -1;
    _socketDescriptor = -1;
    _server = NULL;
}
String Socket::getLocalIP()
{
	String txtIP;
	struct hostent *phe = gethostbyname(getLocalHost());
	struct in_addr addr;

	int i = 0;
	if (phe->h_addr_list[i+1])
		i++;
	bcopy(phe->h_addr_list[i],(char*) &addr, sizeof(addr));

	return strIP((unsigned char*)&addr.s_addr);
}

String Socket::strIP(unsigned char* ip)
{
	String txtIP;
	for (int i = 0; i < 3;i++)
	{
		txtIP+=String::toBaseString((int)ip[i],10) + ".";
	}
	txtIP+=String::toBaseString((int)ip[3],10);
	return txtIP;
}
String Socket::getRemoteIP()
{
    String txtIP;
    struct sockaddr_in clientAddr;
    socklen_t lenClient = sizeof(clientAddr);
    getpeername(_socketDescriptor, (struct sockaddr*)&clientAddr, &lenClient);
    return strIP((unsigned char*)&clientAddr.sin_addr.s_addr);
}
int Socket::getRemotePort()
{
	 struct sockaddr_in clientAddr;
	 socklen_t lenClient = sizeof(clientAddr);
	 getpeername(_socketDescriptor, (struct sockaddr*)&clientAddr, &lenClient);
	 return clientAddr.sin_port;
}
String Socket::getLocalHost()
{
	char hostname[80];
	gethostname(hostname, 80);
	return hostname;
}

void Socket::close()
{
    if (_socketDescriptor != -1)
    {
        ::close(_socketDescriptor);
        _socketDescriptor = -1;
    }
}

void Socket::connect(String host, int port)
{
    close();
    struct sockaddr_in servAddr;
    _server = gethostbyname(host);//La memoria es liberada cuando el hilo termina, segun la documentacion
    _socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (_socketDescriptor == -1)
        throw SocketException("Error creando socket");
    servAddr.sin_family = AF_INET;
    bcopy((char *)_server->h_addr,
         (char *)&servAddr.sin_addr.s_addr,
         _server->h_length);
    //servAddr.sin_addr.s_addr = _server->h_addr;//direccion ip
    servAddr.sin_port = htons(port);
    if (::connect(_socketDescriptor, (struct sockaddr *)&servAddr, sizeof(servAddr))==-1)
        throw SocketException("No se pudo conectar con el servidor");

    _host = host;
    _port = port;
}
void Socket::bcopy(char* src, char* dest, int len)
{
    for (int i = 0; i < len; i++)
    {
        dest[i] = src[i];
    }
}
int Socket::read(char*dest, int length)
{
    if (_socketDescriptor == -1)
        throw SocketException("Conexion cerrada");
    int totalRec = 0;
    int bytesRec = 0;
    while(totalRec < length)
    {
        bytesRec = ::read(_socketDescriptor, dest + totalRec, length - totalRec);
        switch(bytesRec)
        {
            case 0:
                _socketDescriptor = -1;
                return totalRec;
            case -1:
                _socketDescriptor = -1;
                throw SocketException("Error en recepcion");
            default:
                totalRec+=bytesRec;
                break;
        }
    }
    return totalRec;
}
void Socket::write(String data)
{
	write(data, data.length());
}
void Socket::write(const char*data, int length)
{
    if (_socketDescriptor==-1)
        throw SocketException("Conexion cerrada");
    int totalEnv = 0;
    int bytesEnv = 0;
    //Mientras no se envien todos los datos;
    while(totalEnv < length)
    {
        bytesEnv = ::write(_socketDescriptor, data + totalEnv, length - totalEnv);
        switch(bytesEnv)
        {
            case 0:
                _socketDescriptor = -1;
                throw SocketException("Conexion cerrada");
            case -1:
                _socketDescriptor = -1;
                throw SocketException("Error en el envio");
            default:
                totalEnv+=bytesEnv;
                break;
        }
    }

}
#endif // SOCKET_H
