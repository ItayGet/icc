#include "parser-type.h"

#include <stdlib.h>

void castIrArgBasicType(ExprRet *er, ScopeContext *sc, BasicType castType) {
	if(er->type->basic == castType) { return; }

	IrProg *castProg = malloc(sizeof(IrProg));
	IrInstr *castInstr = &castProg->val;

	castInstr->action = actionCast;
	castInstr->type = castType;
	castInstr->b = *er->arg;

	er->arg->type = argInstr;
	er->arg->i = &castProg->val;

	*sc->prog = castProg;
	sc->prog = &castProg->next;
}

BasicType promoteInteger(BasicType a, BasicRank *rank) {
	// Integer promotions don't change types whose ranks are bigger than
	// int
	if(*rank > GET_BASIC_RANK(basicInt)) { return a; }
	
	// FIXME: Check if all values of a can be represented by an int and if
	// they don't, return unsigned int
	*rank = GET_BASIC_RANK(basicInt);
	return a == basicUInt ? basicUInt : basicInt;
}

BasicType getUsualArithSingedUnsigned(BasicType signedType, BasicType unsignedType,
				      int sRank, int uRank) {
	if(uRank >= sRank) {
		return unsignedType;
	}

	// FIXME: Use types' sizes to determine if the unsigned type could be
	// held inside the signed type
	return getUnignedBasicType(signedType);
}

BasicType getUsualArithConversion(BasicType a, BasicType b) {
	BasicTypeType ta = GET_BASIC_TYPE_TYPE(a), tb = GET_BASIC_TYPE_TYPE(b);

	if(IS_ANY_TYPE_EQUAL(ta, tb, basicTypeError)) { /* error */ }
	
	BasicRank aRank = GET_BASIC_RANK(a);
	BasicRank bRank = GET_BASIC_RANK(b);

	if(IS_ANY_TYPE_EQUAL(ta, tb, basicTypeFloat)) {
		return aRank > bRank  ? a : b;
	}

	// Integer promotion rules
	a = promoteInteger(a, &aRank);
	b = promoteInteger(b, &bRank);

	if(ta == tb) {
		return aRank > bRank  ? a : b;
	}

	BasicType ret;
	// By deducing the state of the function, one type is signed and the
	// other is unsigned
	if(ta == basicTypeSigned) {
		ret = getUsualArithSingedUnsigned(a, b, aRank, bRank);
	} else {
		ret = getUsualArithSingedUnsigned(b, a, bRank, aRank);
	}
	return ret;
}

void castAssignmentExpression(ExprRet *rhs, ScopeContext *sc, Type *lhsType) {
	// TODO: Check for a non-const l-value

	switch(lhsType->type) {
	case typeRecord:
		if(!areTypesEqual(rhs->type, lhsType)) { /* error */ }
		break;
	case typePointer:
		if(rhs->type->type == typeArray || rhs->type->type == typePointer) {
			/* error */
		}
		break;
	case typeBasic:
		if(rhs->type->type != typeBasic) { /* error */ }
		castIrArgBasicType(rhs, sc, lhsType->basic);
	case typeArray: /* error */;
	case typeFunction: /* error */;
	case typeError: /* error */;
	}
}

Type *doUsualArithConversion(ExprRet *lhs, ExprRet *rhs, ScopeContext *sc) {
	BasicType basic = getUsualArithConversion(lhs->type->basic, rhs->type->basic);
	castIrArgBasicType(lhs, sc, basic);
	castIrArgBasicType(rhs, sc, basic);

	// TODO: For every if statement in this function, add a check for
	// whether the type in question has no qualifiers

	// Try to keep resources for the types
	if(rhs->type->basic == basic) {
		// No memory changed because rhs is given as the new expression
		// type and lhs should not be cleaned
		return rhs->type;
	}
	
	// If rhs will not be used as the return value, it should be cleaned
	cleanType(rhs->type);

	if(lhs->type->basic == basic) { 
		// lhs will now both stay as lhs and be returned
		increaseReferencesType(lhs->type);
		return lhs->type;
	}

	// The type of the expression is a type that does not exist so it should be initialized 

	Type *t = malloc(sizeof(Type));
	makeType(t);
	t->type = typeBasic;
	t->basic = basic;
	return t;
}

Type *castAdditiveExpression(ExprRet *lhs, ExprRet *rhs, ScopeContext *sc, bool isSub) {
	TypeType lhsT = lhs->type->type, rhsT = rhs->type->type;

	// These types are not applicable for arithmetic
	if(IS_ANY_TYPE_EQUAL(lhsT, rhsT, typeRecord)) { /* error */ }
	if(IS_ANY_TYPE_EQUAL(lhsT, rhsT, typeFunction)) { /* error */ }
	if(IS_ANY_TYPE_EQUAL(lhsT, rhsT, typeError)) { /* error */ }

	if(IS_ANY_TYPE_EQUAL(lhsT, rhsT, typePointer) || 
	   IS_ANY_TYPE_EQUAL(lhsT, rhsT, typeArray)
			) {
		// Pointer arithmetic

		// Because there has to be a pointer if any operand is an integer, then
		// pointer arithmetic is valid. If there is no integer type
		// then types are not compatible for arithmetic
		if(lhsT == typeBasic && IS_INTEGER(lhs->type->basic)) {
			// lhs should always be the pointer in pointer
			// arithmetic so we should swap rhs and lhs
			ExprRet *tmp = lhs;
			lhs = rhs;
			rhs = tmp;
		} else if(lhsT == typeBasic && IS_INTEGER(rhsT)) {
			// Nothing to do, since lhs is the pointer
		} else if(isSub &&
				((lhsT == typePointer || lhsT == typeArray) &&
				 (rhsT == typePointer || rhsT == typeArray))
			 ) {
			// Pointer subtraction
			
			// FIXME: change to ptr_diff type
			Type *diffType = malloc(sizeof(Type));
			makeType(diffType);
			diffType->type = typeBasic;
			diffType->basic = basicInt;

			cleanType(rhs->type);
			return diffType;
		} else { /* error */ }

		// TODO: Get the size of the pointee in lhs, add IR instructions for multiplying it by rhs and casting rhs to be size_t
		
		cleanType(rhs->type);
		return lhs->type;
	} else if(lhsT == typeBasic && rhsT == typeBasic) {
		return doUsualArithConversion(lhs, rhs, sc);
	} else { /* error */ }
	return NULL;
}
