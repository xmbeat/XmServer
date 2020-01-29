#ifndef BASE64_H
#define BASE64_H
#include "../String/String.h"
#ifndef _BYTE_
#define _BYTE_
typedef unsigned char byte;
#endif
class Base64
{
    private:
        static char TABLE_64[];
        static byte INDEX_TABLE[];
        char _padChar;
        static class _init
        {
            public:
            _init()
            {
                for (int i = 0; i < 256; i++)
                    INDEX_TABLE[i] = 0;//puede estar de mas, pero por si...
                for (int i = 0; i < 64; i++)
                    INDEX_TABLE[(int)TABLE_64[i]] = (byte)i;
            }
        }_initializer;
    public:
        virtual ~Base64(){}
        Base64(char padChar = '=')
        {
            _padChar = padChar;
        }
        virtual String decode(String data)
        {
            int realLen = data.length() - 1;
            while(data.charAt(realLen)==_padChar) realLen--;
            realLen++;
            String result(realLen * 3 / 4);//aprox.
            byte rightBits = 0;
            byte leftBits = 0;
            int shift = 2;
            int index = 0;
            bool procesado = false;

            for (int i = 0; i < realLen; i++)
            {
                byte curByte = INDEX_TABLE[(int)data.charAt(i)];
                rightBits = curByte >> (8 - shift);
                if (procesado)
                {
                    result.setChar(index++, rightBits | leftBits);
                }
                leftBits = curByte << shift;
                if (shift == 8){
                    procesado = false;
                    shift = 2;}
                else{
                    procesado = true;
                    shift+=2;
                }
            }
            return result;
        }
        virtual void setPadChar(char padChar)
        {
            _padChar = padChar;
        }
        virtual String encode(String data)
        {
            typedef unsigned char byte;
            int shift = 2;
            int finalLen = data.length() / 3 * 4;
            if (data.length() % 3) finalLen += 4;
            String string64(finalLen);
            byte leftBits = 0;
            byte rightBits = 0;
            int index = 0;
            for (int i = 0; i < data.length(); i++)
            {
                byte curByte = data.charAt(i);
                leftBits = rightBits | (curByte >> shift);
                string64.setChar(index++, TABLE_64[leftBits]);
                rightBits = (curByte << (6 - shift)) & 0x3F;
                if (shift == 6 || i == data.length() - 1)
                {
                    string64.setChar(index++, TABLE_64[rightBits]);
                    rightBits = 0x00;
                    shift = 2;
                }
                else shift += 2;
            }

            for (; index < finalLen; index++)
            {
                string64.setChar(index, _padChar);
            }
            return string64;
        }
};
char Base64::TABLE_64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
byte Base64::INDEX_TABLE[256];
Base64::_init Base64::_initializer;
#endif // BASE64_H
