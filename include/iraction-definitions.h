// STR is a string of how the action would be displayed as a printf format
#ifndef ACTION
#define ACTION(NAME, STR)
#endif

ACTION(Error, "ERROR")

// Binary operations(require 2 operands)
ACTION(Add, "t = %s + %s")
ACTION(Sub, "t = %s - %s")
ACTION(Mul, "t = %s * %s")
ACTION(Div, "t = %s / %s")
ACTION(Mod, "t = %s % %s")
ACTION(Assign, "%s = %s")
ACTION(AssignDerefR, "*%s = %s")
ACTION(AssignArray, "t = %s[%s]")

ACTION(CompEqual, "if %s == %s")
ACTION(CompNotEqual, "if %s != %s")
ACTION(CompLessThan, "if %s < %s")
ACTION(CompGreaterThan, "if %s > %s")
ACTION(CompLessThanEqual, "if %s <= %s")
ACTION(CompGreaterThanEqual, "if %s >= %s")

ACTION(Cast, "(%d)%s")

// A separator between binary and unary operations
ACTION(Separator, "SEP")

// Unary operations
ACTION(UAssign, "t = %s")
ACTION(AssignDerefL, "t = *%s")
ACTION(Param, "param %s")
ACTION(Call, "call %s")
ACTION(UAdd, "t = +%s")
ACTION(USub, "t = -%s")
ACTION(Ref, "t = &%s")
ACTION(Goto, "goto %s")

ACTION(CondGoto, "condgoto %s")

#undef ACTION
