	
#ifndef TYPE
#define TYPE(NAME, STR)
#endif

#ifndef BASIC_TYPE
#define BASIC_TYPE(NAME, STR) TYPE(NAME, STR)
#endif

TYPE(Error, "ERROR")

// Record type used for unions and structs
TYPE(Record, "{ %s }")
TYPE(Pointer, "%s*")
TYPE(Array, "%s[%d]")
TYPE(Function, "%s(%s)")

// Seperate basic types from complex ones
TYPE(Separator, "SEP")

BASIC_TYPE(Void, "void")
BASIC_TYPE(Char, "char")
BASIC_TYPE(UChar, "unsigned char")
BASIC_TYPE(SInt, "short int")
BASIC_TYPE(USInt, "unsigned short int")
BASIC_TYPE(Int, "int")
BASIC_TYPE(UInt, "unsigned int")
BASIC_TYPE(LInt, "long int")
BASIC_TYPE(ULInt, "unsigned long int")
BASIC_TYPE(LLInt, "long long int")
BASIC_TYPE(ULLInt, "unsigned long long int")
BASIC_TYPE(Float, "float")
BASIC_TYPE(Double, "double")
BASIC_TYPE(LDouble, "long double")

#undef BASIC_TYPE
#undef TYPE
