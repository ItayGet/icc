// A parser for expression that uses the AST from exprast.h

#pragma once

#include "exprast.h"
#include "lex.h"


// Parses all expressions in greater precedence than binary expressions
ExprAst *parseCastExpression(TokenStream *ts);

// Parses binary expressions
// prec is the minimum precedence that will be parsed
ExprAst *parseBinaryExpression(TokenStream *ts, OperatorPrec prec);

// Parses conditional(AKA ternary) expressions
ExprAst *parseConditionalExpression(TokenStream *ts);

// This macro acts like a function
#define parseExpression(ts) parseBinaryExpression((ts), precComma)
