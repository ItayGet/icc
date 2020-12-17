// Definitions of basic and complex types
#ifndef TYPE
#define TYPE(NAME, STR)
#endif

// Order is the dictating whether implicit conversions are possible
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

// Basic types are ordered by size, usesful for implicit conversions
BASIC_TYPE(Void, "void")
BASIC_TYPE(Char, "char")
BASIC_TYPE(SInt, "short int")
BASIC_TYPE(Int, "int")
BASIC_TYPE(LInt, "long int")
BASIC_TYPE(LLInt, "long long int")
BASIC_TYPE(Float, "float")
BASIC_TYPE(Double, "double")
BASIC_TYPE(LDouble, "long double")

#undef BASIC_TYPE
#undef TYPE
