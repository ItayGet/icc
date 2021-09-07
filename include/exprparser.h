// A parser for expression that uses the AST from exprast.h

#pragma once

#include "exprast.h"
#include "lex.h"

ExprAst *parseCastExpression(Stream *s);
ExprAst *parseBinaryExpression(Stream *s, OperatorPrec prec);
