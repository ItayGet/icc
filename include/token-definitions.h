// Define a token that can be redefined when included
#ifndef TOKEN
#define TOKEN(NAME)
#endif

// STR is defined for documentation and changes nothing
#ifndef PUNC
#define PUNC(NAME, STR) TOKEN(punc ## NAME)
#endif

PUNC(Error, "")

// Brackets
PUNC(LSBracket, "[")
PUNC(RSBracket, "]")
PUNC(LRBracket, "(")
PUNC(RRBracket, ")")
PUNC(LCBracket, "{")
PUNC(RCBracket, "}")

// Misc. single-char punctuators
PUNC(Tilde, "~")
PUNC(Comma, ",")
PUNC(Dot, ".")
PUNC(Semicolon, ";")
PUNC(Que, "?")
PUNC(Colon, ":")
	
//Operators
PUNC(Plus, "+")
PUNC(DPlus, "++")
PUNC(EPlus, "+=")

PUNC(Minus, "-")
PUNC(DMinus, "--")
PUNC(EMinus, "-=")
PUNC(Arrow, "->")

PUNC(Asterisk, "*")
PUNC(EAsterisk, "*=")

PUNC(Slash, "/")
PUNC(ESlash, "/=")

PUNC(Equal, "=")
PUNC(DEqual, "==")

PUNC(Percent, "%")
PUNC(EPercent, "%=")

PUNC(Amp, "&")
PUNC(DAmp, "&&")
PUNC(EAmp, "&=")

PUNC(Bar, "|")
PUNC(DBar, "||")
PUNC(EBar, "|=")

PUNC(LT, "<")
PUNC(DLT, "<<")
PUNC(ELT, "<=")
PUNC(DELT, "<<=")

PUNC(GT, ">")
PUNC(DGT, ">>")
PUNC(EGT, ">=")
PUNC(DEGT, ">>=")

PUNC(Excl, "!")
PUNC(EExcl, "!=")

PUNC(Caret, "^")
PUNC(ECaret, "^=")

#undef TOKEN
#undef PUNC
