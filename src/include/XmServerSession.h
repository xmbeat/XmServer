#ifndef XMSERVERSESSION_H
#define XMSERVERSESSION_H
#include "Thread/Thread.h"
#include "SessionUtils.h"
#include "File/File.h"
using namespace SessionUtils;
class XmServerSession: public Thread
{
	private:
		Socket _socket;
		bool _isDinamic;
		bool _isActiveSession;
		XHeaderCallback mFnHeader;
		XProgressFunc mFnProg;
		XLoginCallback mFnLogin;
		int ID;
		bool isValidUser(String user, String pass)
		{
			return true;
		}

    public:
        XmServerSession(Socket socket, bool isDinamic,
        		int ID, XHeaderCallback fn = NULL,
        		XProgressFunc prog = NULL, XLoginCallback login = NULL)
        {
        	_isActiveSession = true;
            _socket = socket;
            _isDinamic = isDinamic;
            mFnHeader = fn;
            mFnProg = prog;
            mFnLogin = login;
            this->ID = ID;
        }
        virtual void run();
        virtual void processCommand(MetadataList&);

        virtual ~XmServerSession() {}
};
void XmServerSession::run()
{
	String user;
	String pass;
	bool isLogin = false;
    //std::cout << _socket.getRemoteHost() << std::endl;
    try
    {
    	MetadataList header = extractMetadata(&_socket);
    	user = header[0].getName();
    	pass = header[0].getValue();
    	if (isValidUser(user, pass))
    	{
    		if (mFnLogin) mFnLogin(ID, user, true);
    		isLogin = true;
			while(_isActiveSession)
			{
				header = extractMetadata(&_socket);
				if (mFnHeader) mFnHeader(ID, &header);
				processCommand(header);
			}

    	}
    	else
    	{
    		header.clear();
    		header.add(APP_NAME, "BAD");
    		header.add("Error:", "Usuario o contrasena no validos");
    		sendPackage(&header, NULL, &_socket);
    	}
    }
    catch(Exception &e)//NO DEBERIA LLEGAR AQUI
    {
    	std::cout << e.getMessage() << std::endl;
    }
    catch(...)//TODAVIA MENOS!
    {
    	std::cout << "Unexpected error!\n";
    }
    if (isLogin && mFnLogin) mFnLogin(ID, user, false);
    std::cout << String("Finalizando: ") + getName() + "\n";
    _socket.close();

    if (_isDinamic) delete this;//Nos borramos a nosotros mismos,
    //para que no exista problemas de memoria, aunque se puede comparar una
    //variable de stack con el puntero this y si this es mayor, entonces es dinamica
}

void XmServerSession::processCommand(MetadataList &metadatos)
{
	if (!metadatos.size())
	{
		_isActiveSession = false;
		return;
	}
	MetadataList head;
	String command = metadatos.get(0).getName();
	String parametro = metadatos.get(0).getValue();
	if (command.equals("GET"))
	{
		int bytePos = 0;
		HeadAttrib *attrib = metadatos.getAttribByName("BYTE-POS");
		if (attrib != NULL){
			bytePos = attrib->getIntValue();
			delete attrib;
		}
		try{
			BodyData body(parametro, bytePos, -1);
			head.add(APP_NAME, "GOOD");
			sendPackage(&head, &body, &_socket);
		}
		catch(SocketException &e)
		{
			_isActiveSession = false;
		}
		catch(Exception &e){
			head.add(APP_NAME, "BAD");
			try
			{
				sendPackage(&head, NULL, &_socket);
			}
			catch(...)
			{
				_isActiveSession = false;
			}
		}

	}
	else if (command == "CD")
	{
		if (File::setWorkDir(parametro))
		{
			head.add(APP_NAME, "GOOD");
		}
		else
		{
			head.add(APP_NAME, "BAD");
		}
		sendPackage(&head, NULL, &_socket);
	}
	else if (command == "LS")
	{
		if (parametro.charAt(parametro.length()-1) != '/')
			parametro+="/";
		File file(parametro);
		if (file.isDirectory() && file.canRead())
		{
			head.add(APP_NAME, "GOOD");
			ArrayList<String> dirs = file.list();
			head.add("FILE-COUNT", String::toBaseString(dirs.size(), 10));
			String attribs(4);
			for (int i = 0; i < dirs.size();i++)
			{

				File curFile(parametro+dirs[i]);
				if (curFile.isDirectory())
					attribs.setChar(0,'d');
				else
					attribs.setChar(0,'-');
				if (curFile.canRead())
					attribs.setChar(0,'r');
				else
					attribs.setChar(0,'-');
				if (curFile.canWrite())
					attribs.setChar(0,'w');
				else
					attribs.setChar(0,'-');
				head.add(attribs, dirs[i]);
			}
			sendPackage(&head, NULL, &_socket);
		}
		else
		{
			head.add(APP_NAME, "BAD");
			sendPackage(&head, NULL, &_socket);
		}
	}
	else if(command == "PWD")
	{
		head.add(APP_NAME, "GOOD");
		head.add(getOSInfo(), File::getWorkDir());
		sendPackage(&head, NULL, &_socket);
	}
	else if (command == "DEL")
	{
		File file(parametro);
		if (file.remove())
			head.add(APP_NAME, "GOOD");
		else
			head.add(APP_NAME, "BAD");
		sendPackage(&head, NULL, &_socket);
	}
	else if (command == "MKDIR")
	{
		File file(parametro);
		if (file.mkdir())
			head.add(APP_NAME, "GOOD");
		else
			head.add(APP_NAME, "BAD");
		sendPackage(&head, NULL, &_socket);
	}
	else if (command == "PUT")
	{
		HeadAttrib *attrib = metadatos.getAttribByName("CONTENT-LENGTH");
		if (attrib != NULL){
			int length = attrib->getIntValue();
			if (saveResource(&_socket,length, parametro))
				head.add(APP_NAME, "GOOD");
			else
				head.add(APP_NAME, "BAD");
		}
		else
		{
			head.add(APP_NAME, "BAD");
		}
		sendPackage(&head, NULL, &_socket);
	}
	else if (command.equals("QUIT"))
	{
		_isActiveSession = false;
	}
}
#endif // XMSERVERSESSION_H
