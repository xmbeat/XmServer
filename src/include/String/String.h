#ifndef CADENA_H
#define CADENA_H
#define null 0

class Cadena {
private:
	int longitud;
	char *cadena;
	static double pow(int, int);
	void init(const int);
public:
	Cadena() :
			cadena(null) {
		longitud = 0;
	}
	Cadena(const unsigned int size);
	Cadena(const int size);
	Cadena(const char *c);
	Cadena(const char c);
	Cadena(const Cadena &c);
	~Cadena();
	Cadena &operator=(const Cadena &c);
	Cadena operator+(const Cadena &c);
	Cadena operator+(const int &i);
	bool operator==(const Cadena &c); //comprueba toda la cadena
	bool operator==(const char *c);
	bool operator!=(const Cadena &c);
	bool operator!=(const char *c);
	Cadena &operator+=(const Cadena &c);
	operator const char *() {
		return cadena;
	}
	;
	int length() {
		return longitud;
	}
	;
	Cadena substring(int);
	Cadena substring(int beginIndex, int len);
	char charAt(int index);
	void setChar(int index, const char &c);
	bool equals(const char *c, bool ignoreCase = false);
	bool equals(const Cadena &c, bool ignoreCase = false);
	int inString(const char *c, int index = 0);
	int inString(const Cadena &c, int index = 0); //estoy checando en saber cuando el BOM es factible
	void replace(int start, int len, Cadena &c);
	void replace(int start, int len, char *c);
	int strLen(const char *c);
	char* strCopy(char *dest, const char *origen);
	char* strCopy(char *dest, const char *origen, int inicio, int index,
			int len = 0, bool addZero = false);
	static Cadena toBaseString(int, int);
	static int toInteger(Cadena, int);
	static double strVal(const char *);
	static Cadena strStr(double);
	static Cadena string(char, int);
};
bool Cadena::operator!=(const char *c) {
	return !(*this == c);
}
bool Cadena::operator !=(const Cadena &c) {
	return !(*this == c);
}
void Cadena::init(const int size) {
	if (size > 0) {
		longitud = size;
		cadena = new char[size + 1];
		for (int i = 0; i <= size; i++)
			cadena[i] = 0;
	} else {
		longitud = 0;
		cadena = null;
	}
}
Cadena::Cadena(const int size) {
	init(size);
}

Cadena::Cadena(const unsigned int size) {
	init(size);
}

Cadena::Cadena(const char *c) {
	longitud = strLen(c);
	if (longitud) {
		cadena = new char[longitud + 1];
		strCopy(cadena, c, 0, 0, 0, true);
	} else {
		cadena = null;
	}
}
Cadena::Cadena(const char c) {
	longitud = 1;
	cadena = new char[2];
	cadena[0] = c;
	cadena[1] = 0;
}
Cadena::Cadena(const Cadena &c) :
		cadena(null) {
	*this = c;
}

Cadena::~Cadena() {
	delete[] cadena;
}
Cadena Cadena::operator +(const int& i) {
	return *this + Cadena::toBaseString(i, 10);
}
Cadena Cadena::operator +(const Cadena &c) {
	int longA, longB;
	Cadena temp;
	longA = longitud;
	longB = c.longitud;
	temp.longitud = longA + longB;
	if (temp.longitud > 0) {
		temp.cadena = new char[temp.longitud + 1];
		strCopy(temp.cadena, cadena, 0, 0, longA);
		strCopy(temp.cadena, c.cadena, longA, 0, longB, true);
	} else {
		temp.longitud = 0;
		temp.cadena = null;
	}

	return temp;
}

Cadena &Cadena::operator=(const Cadena &c) {
	if (this != &c) {
		delete[] cadena;
		longitud = c.longitud;

		if (longitud) {
			cadena = new char[longitud + 1];

			strCopy(cadena, c.cadena, 0, 0, longitud, true);

		} else {
			cadena = null;
		}
	}
	return *this;
}
bool Cadena::operator==(const char *c) {
	return *this == Cadena(c);
}

bool Cadena::operator==(const Cadena &c) {
	int b = c.longitud;
	int a = longitud;
	if (a == b) {
		while (a--)
			if (this->cadena[a] != c.cadena[a])
				return false;

		return true;
	}
	return false;
}

Cadena &Cadena::operator+=(const Cadena &c) {
	return (*this = *this + c);
}

int Cadena::strLen(const char *c) {
	if (c == null)
		return 0;
	int i = 0;
	while (c[i++])
		;
	return (i - 1);
}

char *Cadena::strCopy(char *dest, const char *origen) {
	int i = 0;
	if (origen != null) {
		while (origen[i]) {
			dest[i] = origen[i];
			i++;
		}
	}
	return dest;
}

char *Cadena::strCopy(char *dest, const char *origen, int inicio, int index,
		int len, bool addZero) {
//inicio=2//index 7 len 20
	if (origen != null) {
		if (len == 0) {
			while (origen[index]) {
				dest[inicio++] = origen[index++];
			}
		} else {
			while (len-- > 0) { //19-0
				dest[inicio++] = origen[index++];
			}
		}
	}
	if (addZero) {
		dest[inicio] = 0;
	}
	return dest;
}
Cadena Cadena::substring(int index) {
	return substring(index, longitud - index);
}

Cadena Cadena::substring(int index, int len) {
	Cadena temp;
	if (index >= 0 && index < longitud) {
		if (len > 0) {
			len = index + len > longitud ? longitud - index : len; //en caso de que la longitud sea mayor al maximo esperado lo limito
			temp.longitud = len;
			temp.cadena = new char[len + 1];
			strCopy(temp.cadena, cadena, 0, index, len, true);
		}
	}
	return temp;
}
/*regresa un char segun su indice en la cadena, no comprueba
 la existencia de la cadena ni su longitud, la debe hacer el programador*/
char Cadena::charAt(int index) {
	return cadena[index];
}
void Cadena::setChar(int index, const char &c) {
	cadena[index] = c;
}
bool Cadena::equals(const char *c, bool ignoreCase) {
	return equals(Cadena(c), ignoreCase);
}

bool Cadena::equals(const Cadena &c, bool ignoreCase) {
	int i = 0;
	unsigned char byte, cmp;
	if (longitud != c.longitud)
		return false;
	i = longitud;
	if (ignoreCase) {
		while (i--) {
			byte = c.cadena[i];
			cmp = cadena[i];
			if (byte > 64 && byte < 91) { //A-Z
				if ((byte != cmp) && ((byte + 32) != cmp)) {
					return false;
				}
			} else if (byte > 96 && byte < 123) { //a-z
				if ((byte != cmp) && ((byte - 32) != cmp)) {
					return false;
				}
			} else if (byte > 191 && byte < 222) { //vocales con acento ñ's etc.
				if ((byte != cmp) && ((byte + 32) != cmp)) {
					return false;
				}
			} else if (byte > 223 && byte < 255) { //no estoy seguro en estos ultimos caracteres!, quiza deben extender 1 byte mas
				if (byte != cmp && ((byte - 32) != cmp)) {
					return false;
				}
			}
		}
		return true;
	} else {
		while (i--) {
			if (cadena[i] != c.cadena[i])
				return false;
		}
		return true;
	}
	return false;
}

int Cadena::inString(const char *c, int index) {
	return inString(Cadena(c), index);
}

int Cadena::inString(const Cadena &c, int index) {
	int limite = longitud - c.longitud + 1;
	int i, j;
	if (index < 0)
		index = 0;
	for (i = index; i < limite; i++) {
		j = 0;
		while (j < c.longitud) {
			if (cadena[i + j] != c.cadena[j])
				break; //j=c.longitud;
			if (++j == c.longitud)
				return i;
		}
	}
	return -1;
}
void Cadena::replace(int start, int len, char *c) {
	Cadena temp(c);
	replace(start, len, temp);
}
void Cadena::replace(int start, int len, Cadena &c) {
	char *d = null;
	int size, i;
	if (start > longitud || start < 0)
		return;
	if (start + len > longitud) { //si se excede la longitud de la cadena
		len = longitud - start;
	}
	if (len == c.longitud) { //si son de misma longitud no tiene caso volver a reservar memoria
		while (len--) {
			cadena[start + len] = c.cadena[len];
		}
	} else { //hay que eliminar y reservar memoria
		size = longitud - len + c.longitud;
		if (size > 0) {
			d = new char[size + 1]; //reservamos memoria
			if (start > 0)
				strCopy(d, cadena, 0, 0, start);
			strCopy(d, c.cadena, start, 0, c.longitud);
			if ((i = longitud - start - len) > 0)
				strCopy(d, cadena, start + c.longitud, start + len, i, true);
			delete[] cadena;
			longitud = size;
			cadena = d;
		}
	}
}

double Cadena::pow(int num, int potencia) { //solo usado para no depender de Math
	double resultado = 1;
	for (int i = 0; i < potencia; i++) {
		resultado *= num;
	}
	for (int i = 0; i > potencia; i--) {
		resultado /= num;
	}
	return resultado;
}
int Cadena::toInteger(Cadena str, int base) {
	int ret = 0;
	for (int i = 1; i <= str.length(); i++) {
		char curChar = str.charAt(i - 1);
		if (curChar >= '0' && curChar <= '9') //Es numero
			curChar -= '0';
		else if (curChar >= 'a' && curChar <= 'z') //Alfanumerico minuscula
			curChar -= 'a' - 10;
		else if (curChar >= 'A' && curChar <= 'Z')
			curChar -= 'A' - 10;
//else //error, caracter no soportado
		ret += curChar * pow(base, str.length() - i);
	}
	return ret;
}

Cadena Cadena::toBaseString(int number, int base) {
	if (!number)
		return "0";
	typedef unsigned char byte;
	byte size = 0;
	unsigned int rest = number;
	while (rest) {
		size++;
		rest /= base;
	}
	Cadena ret((unsigned int) size);
	for (byte i = 1; i <= size; i++) {
		byte residuo = number % base;
		number /= base;
		if (residuo < 10)
			ret.setChar(size - i, '0' + residuo);
		else
			ret.setChar(size - i, 0x57 + residuo);
	}
	return ret;
}

double Cadena::strVal(const char *c) {
	int i = 0, digito, potencia = 6, dot = 0;
	double ret = 0;
	bool positive = true;
	while (c[i]) {
		if (c[i] > 47 && c[i] < 58) { //digito
			digito = c[i] - 48;
			ret += digito * pow(10, potencia--);
		} else if (c[i] == 46 && !dot) { //"."
			dot = potencia;
		} else if (c[i] == 45 && ret == 0) { //"-"
			positive = !positive;
		} else if (c[i] != 43 || ret != 0) {
			break;
		}
		i++;
	}
	if (!dot)
		dot = potencia;
	ret *= pow(10, -potencia - 1 - dot + potencia);
	return positive ? ret : -ret;
}

Cadena Cadena::strStr(double numero) {
	Cadena temp;
	long int residuo, i = 10, cociente;
	char c;
	while (int(numero) != 0) {
		residuo = ((int) numero) % i;
		numero -= residuo;
		residuo /= (i / 10);
		c = residuo + 48;
		temp = Cadena(c) + temp;
		i *= 10;
	}
	if (numero != 0) {
		temp += '.';
		i = 10;
		while (numero != 0) {
			numero *= 10;
			cociente = numero;
			residuo = cociente % 10;
			c = residuo + 48;
			temp += c;
			if (numero > 0)
				numero -= residuo;
		}
	}
	if (!temp.length())
		temp = "0";
	return temp;
}
Cadena Cadena::string(char c, int count) {
	Cadena ret(count);
	for (int i = 0; i < count; i++) {
		ret.setChar(i, c);
	}
	return ret;
}
typedef Cadena String;
#endif
