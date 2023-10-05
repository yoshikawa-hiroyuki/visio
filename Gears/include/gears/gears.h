#ifndef GEARS_GEARS_H
#define GEARS_GEARS_H

#include <cctype>
#include <cstddef>
#include <string>

namespace gears {

//typedef unsigned char Bool;
typedef char Char;
typedef wchar_t WChar;
typedef signed char Byte;
typedef unsigned char UByte;
typedef short Short;
typedef unsigned short UShort;
typedef int Int;
typedef unsigned int UInt;
#ifdef WIN32
	typedef __int64 Long;
	typedef unsigned __int64 ULong;
#else
	typedef long long Long;
	typedef unsigned long long ULong;
#endif
typedef float Float;
typedef double Double;
typedef std::string String;
typedef std::wstring WString;

} // end of namespace gears

#endif // GEARS_GEARS_H
