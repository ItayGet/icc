// Takes care of string and character literals

#pragma once

#include "lex.h"

#include <stdbool.h>

// Get a new string character from stream
// Return that character
char getStringChar(Stream *s, int c);

// Check if current token is a string literal
// If it is return true and change token
// else, return false
bool isTokenStringLiteral(Token *t, Stream *s, int c);
