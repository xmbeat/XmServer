#ifndef SESSIONUTILS_H
#define SESSIONUTILS_H
#include "String/String.h"
#include "ArrayList/ArrayList.h"
#include "Socket/Socket.h"
#include "Exception/Exception.h"
#include <fstream>
using namespace std;
typedef void (*XProgressFunc)(int,int, int);//ID, val, max
namespace SessionUtils
{
	const String APP_NAME = "XmServer-Linux/1.0";
	const int SIZE_BUFFER = 10240;
	String getOSInfo()
	{
		return "Ubuntu,10.10,Maverick";
	}
    String encodeURI(String uri)
    {
        String ret;
        for (int i = 0; i < uri.length();i++)
        {
            char curChar = uri.charAt(i);
            if ((curChar < 48 || curChar > 57) &&
                (curChar < 65 || curChar > 90) &&
                (curChar < 97 || curChar > 122))
            {
                ret+= String("%") + String::toBaseString(curChar, 16);
            }
			else
			{
				ret += curChar;
			}
        }
		return ret;
    }
	String decodeURI(String uri)
	{
		String ret;
		for (int i = 0; i < uri.length();i++)
		{
			char curChar = uri.charAt(i);
			if (curChar == '%')
			{
				String strCode = uri.substring(i+1, 2);
				char ascii = (char)String::toInteger(strCode, 16);
				ret += ascii;
				i+=2;
			}
			else
			{
				ret += curChar;
			}
		}
		return ret;
	}
	int inline readStream(ifstream *flujo, char* dest, int size)
	{
		flujo->read(dest, size);
		flujo->clear();//limpiar los errores
		return flujo->gcount();
	}
	class BodyData
	{
		private:
			String _srcFile;
			int _bytePos;
			int _length;
			ifstream _entrada;
		public:
			~BodyData()
			{
				_entrada.close();
			}
			BodyData(String srcFile, int bytePos, int length):
				_srcFile(srcFile), _bytePos(bytePos),
				_entrada(_srcFile, ios::binary | ios::in | ios::ate)
			{
				if (!_entrada.good())
					throw Exception(String("Archivo no valido para lectura: ") + _srcFile);
				int len = _entrada.tellg();
				if (length > len - _bytePos || length < 0)
					_length = len - _bytePos;
				else
					_length = length;
			}
			int length()
			{
				return _length;
			}
			void sendData(Socket *salida)
			{
				_entrada.seekg(_bytePos, ios::beg);
				int chunkSize = 0;
				char *buffer = new char[SIZE_BUFFER];
				try
				{
					while((chunkSize = readStream(&_entrada, buffer, SIZE_BUFFER))>0)
					{
						salida->write(buffer, chunkSize);
					}
					delete []buffer;
				}
				catch(SocketException &e)
				{
					delete []buffer;
					throw e;
				}

			}

	};

	class HeadAttrib
    {
        private:
            String _name;
            String _value;
        public:
            HeadAttrib(){}
            HeadAttrib(String name, int value);
            HeadAttrib(String name, String value, bool isEncoded);
            HeadAttrib(String metadata, bool isEncoded);
            HeadAttrib(String metadata);
            bool operator==(const HeadAttrib &head);
            String getName();
            String getEncodedValue();
            String getValue();
            int getIntValue();
            String toString();
    };

    class MetadataList: public ArrayList<HeadAttrib>
    {
        public:
            HeadAttrib* getAttribByName(String name)
            {
                for (int i = 0; i < size(); i++)
                {
                    HeadAttrib attrib = get(i);
                    if (attrib.getName().equals(name, true))
                        return new HeadAttrib(attrib.getName(), attrib.getEncodedValue(), true);
                }
                return NULL;
            }
            virtual String toString()
            {
                String ret;
                for (int i = 0; i < size(); i++)
                {
                    HeadAttrib attrib = get(i);
                    ret += attrib.toString() + "\n";
                }
                ret += "\n";
                return ret;
            }
            virtual bool add(String name, int value)
            {
                return ArrayList<HeadAttrib>::add(HeadAttrib(name, value));
            }
            virtual bool add(String name, String unCodedValue)
            {
                return ArrayList<HeadAttrib>::add(HeadAttrib(name, unCodedValue, false));
            }
    };

    MetadataList extractMetadata(Socket* receiver)
    {
        MetadataList metadata;
        String content(500);
        int lastPos = 0;
        int index = 0;
        int lineCount = 0;

        while (lineCount != 2)
        {
            char tmp;
            receiver->read(&tmp, 1);
            content.setChar(index++, tmp);
            if (tmp == '\n')
            {
                lineCount++;
            }
            else
                lineCount = 0;
            if (index == content.length())
                content += String::string(0x00, 100);
        }

        for (int i = 0; i < index-1;i++)
        {
            if (content.charAt(i) == '\n')
            {
                HeadAttrib attrib(content.substring(lastPos, i - lastPos),true);
                metadata.ArrayList<HeadAttrib>::add(attrib);
                lastPos = i + 1;
            }
        }
        return metadata;
    }

    void sendPackage(MetadataList *header, BodyData *body, Socket *salida)
	{
		if (body != NULL)
		{
			header->add("CONTENT-LENGTH", body->length());
			salida->write(header->toString());
			if (body->length()>0) body->sendData(salida);
		}
		else
		{
			salida->write(header->toString());
		}
	}

    bool saveResource(Socket *entrada, int length, String archivo, bool append = false,
    		XProgressFunc fun = NULL, int ID = 0)
	{
    	ios::openmode modo = append?ios::app:(ios::openmode)0;
    	ofstream file(archivo, ios::binary | ios::out | modo);
    	if (!file.good()) return false;
		char *buffer = new char[SIZE_BUFFER];
		int max = length / SIZE_BUFFER;
		int rest = length % SIZE_BUFFER;
		int i = 0;
		int bytesLeidos = 0;
		try{
			while(i < max)
			{
				bytesLeidos = entrada->read(buffer, SIZE_BUFFER);
				file.write(buffer, bytesLeidos);
				i++;
				if (fun) fun(ID, i*SIZE_BUFFER, length);
			}
			if (rest>0)
			{
				bytesLeidos = entrada->read(buffer, rest);
				file.write(buffer, rest);
				if (fun) fun(ID, length, length);
			}
			file.close();
			return true;
		}catch(Exception &e)
		{
			delete buffer;
			file.close();
		}
		return false;
	}
};

SessionUtils::HeadAttrib::HeadAttrib(String name, int value)
{
    _name = name;
    _value = String::toBaseString(value, 10);
}
SessionUtils::HeadAttrib::HeadAttrib(String name, String value, bool isEncoded)
{
    _name = name;
    _value = isEncoded?value:SessionUtils::encodeURI(value);
}
SessionUtils::HeadAttrib::HeadAttrib(String metadata, bool isEncoded)
{
    int index = metadata.inString(" ");
    _name = metadata.substring(0, index);
    String value = metadata.substring(index + 1);
    _value = isEncoded?value:SessionUtils::encodeURI(value);
}
String SessionUtils::HeadAttrib::getName()
{
    return _name;
}
String SessionUtils::HeadAttrib::getEncodedValue()
{
    return _value;
}
String SessionUtils::HeadAttrib::getValue()
{
    return SessionUtils::decodeURI(_value);
}
int SessionUtils::HeadAttrib::getIntValue()
{
    return String::toInteger(_value, 10);
}
String SessionUtils::HeadAttrib::toString()
{
    return _name + " " + _value;
}
bool SessionUtils::HeadAttrib::operator==(const HeadAttrib &head)
{
    return _name == head._name && _value == head._value;
}

typedef void (*XLoginCallback)(int, String, bool);//ID, User, in/out
typedef void (*XHeaderCallback)(int, SessionUtils::MetadataList*);//ID, encabezadp
#endif // SESSIONUTILS_H
