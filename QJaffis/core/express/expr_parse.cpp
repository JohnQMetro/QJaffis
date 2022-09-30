/////////////////////////////////////////////////////////////////////////////
// Name:        expr_parse.cpp
// Author :     John Q Metro
// Purpose :    Defines functions and stuff for parsing boolean expressions
// Created:     31.12.06
// Conversion to QT Started April 8, 2013
// Updated:     August 6, 2022 (adding whole string matching)
/////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
#ifndef JFEXPRPARSE
#include "expr_parse.h"
#endif
#ifndef LOGGING_H_INCLUDED
  #include "../utils/logging.h"
#endif // LOGGING_H_INCLUDED
#ifndef UTILS1_H_INCLUDED
#include "../utils/utils1.h"
#endif // UTILS1_H_INCLUDED
#ifndef EXPRESSION_H
  #include "../filters/base/expression.h"
#endif // EXPRESSION_H
//----------------------------------
#include <assert.h>
//*****************************************************************************
const QString v_startc = "(~";
const QString v_aftert = ")&|%";
const QString v_stype = "smtqz";
const QString v_idchars = v_stype + "ic";
//============================================================================
// a simple function for looking for chars
bool IsIn(const QChar& src, const QString& place) {
  return place.contains(src);
}
//============================================================================
// the constructor
jfExpParserClass::jfExpParserClass(const QString& insource, bool issimple,jfFilterMap* inlocalmap) {
	rawexp = insource;
	started = false;
	done = false;
	gotten= false;
	tokenizer = NULL;
	theresult = NULL;
	simple = issimple;
	if (simple) assert(inlocalmap==NULL);
	else localmap = inlocalmap;
  fname1 = "size_t jfExpParserClass::ParseExpression()";
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// activation
bool jfExpParserClass::ParseExpression(size_t& outsize) {
  // constants
  const QString fname="jfExpParserClass::ParseExpression";
	// local variables
	bool wasop;
  QString data;
  QChar lastc, currc;
	jfOperandElem* temp;
	// clearing old stuff
	// starting up
	assert(!started);
	wasop = false;
	tokenizer = new jfExprToker(rawexp);
	theresult = new jfElemArray();
	started = true;
	// a special case: everything is empty!
	if (!(tokenizer->TokensLeft())) {
		delete tokenizer;
		tokenizer = NULL;
		done = true;
		outsize = 0;
		return true;
	}
	// we work on the first element
	// we've got an operator or parenthesis
	if (tokenizer->AtOp()) {
		lastc = tokenizer->GetTChar();
		if (!IsIn(lastc,v_startc)) {
      return MakeError("The expression starts with an invalid op!",true);
		}
		// things are okay
		theresult->Append(lastc);
		wasop = true;
	}
	// we've got an operand here
	else {
		data = tokenizer->GetToken();
		temp = MakeOperand(data);
		if (temp==NULL) return false;
		theresult->Append(temp);
	}
	// we work on subseqent elements
	while (tokenizer->TokensLeft()) {
		// we've got an operator
		if (tokenizer->AtOp()) {
			currc = tokenizer->GetTChar();
			// if the preceeding token was an operand, we check for syntax errors
			if (!wasop) {
				if (IsIn(currc,v_startc)) {
          return MakeError("This symbol is not allowed after an operand!",true);
				}
			}
			// if it was an operator, we check for other errors
			else {
        if (lastc==')') {
					if (IsIn(currc,v_startc)) {
            return MakeError("This symbol is not allowed after a closing parentheis!",true);
					}
				}
				else {
					if (!IsIn(currc,v_startc)) {
            return MakeError("This symbol is not allowed after most operators!",true);
					}
				}
			}
			// everything checks out
			theresult->Append(currc);
			wasop = true;
			lastc = currc;
		}
		// we've got an operand
		else {
			// getting data
			data = tokenizer->GetToken();
			// iff non-empty...
      if (!(data.isEmpty())) {
				// operands cannot follow operands
				if (!wasop) {
          return MakeError("Operands cannot follow operands!",true);
				}
				// otherwise, we do a quick check to see if we follow the correct op
        if (lastc==')') {
          return MakeError("Operands cannot follow right parens!",true);
				}
				// everything is ok...
				temp = MakeOperand(data);
				if (temp==NULL) return false;
				theresult->Append(temp);
				wasop = false;
			}
		}
		// the loop continues here
	}
	// the if the last token is an operator, the only one allowed is ')'
	if (wasop) {
    if (lastc!=')') {
      return MakeError("The expression must not end this way!");
		}
	}
	// done!
	delete tokenizer;
	tokenizer = NULL;
	done = true;
	outsize = theresult->size();
	return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// getting the result
jfElemArray* jfExpParserClass::GetResult() {
	assert(done);
	gotten = true;
	return theresult;
}
//--------------------------------------------------------------------------------------
QString jfExpParserClass::GetRawExp() const {
	return rawexp;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// done
jfExpParserClass::~jfExpParserClass() {
	if ((theresult==NULL) && (!gotten)) delete theresult;
	if (tokenizer!=NULL) delete tokenizer;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// when the parser his an error, this method is called, it takes care of cleanup
// before throwing an error message
bool jfExpParserClass::MakeError(const QString& msg, bool back) {
	size_t tokloc;
	assert(started);
	assert(!done);
	// deleting the result
	theresult->DeleteAll();
	delete theresult;
	theresult = NULL;
	// getting info about the loacation
	if (back) tokloc = tokenizer->LastIndex();
	else tokloc = tokenizer->GetIndex();
	// deleting the tokenizer
	delete tokenizer;
	tokenizer = NULL;
	// building the result
  parse_error = "PARSING ERROR in " + fname1 + " at position ";
  parse_error += QString::number(tokloc) + " : " + msg;
	// done
	return false;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfOperandElem* jfExpParserClass::MakeOperand(const QString& source) {
    // constants
    const QString fname = "jfExpParserClass::MakeOperand";
	// local varibales
    QString xsrc = source;
    QString fpart,spart;
	jfOperandElem* res = NULL;
    int semiloc,slen;
    bool istit, iscs;

	// checking to see if this is an embedded filter
    if (FilterTest(xsrc,'#')) {
		// filters are not allowed in simple expressions
		if (simple) {
            MakeError("Filters are not allowed in somple expressions!",true);
            return NULL;
		}
		if (xsrc.length()<3) {
            MakeError("The filter literal is badly formed!",true);
            return NULL;
		}
		// looking for the splitter
        semiloc = FindNonEscaped(xsrc,";",0);
        if (semiloc<0) {
            MakeError("The filter literal is badly formed!",true);
            return NULL;
		}
		// we split
		SplitString(xsrc,semiloc,fpart,spart);
		// initial checking for problems
		if (fpart==EXP_FIL_TAG) {
            MakeError("Expression filter literals are not allowed!",true);
            return NULL;
		}
        if (fpart.isEmpty() || spart.isEmpty()) {
            MakeError("The filter literal is badly formed!",true);
            return NULL;
		}
		// we now work on creating the embedded filter
		res = new jfLiteralFilterElem(fpart,spart);
		if (!(res->IsValid())) {
			delete res;
            MakeError("The embedded filter is problematic!",true);
			return NULL;
		}
	}

	// checking to see if this a filter reference operand
    else if (FilterTest(xsrc,'@')) {
		if (simple) {
            MakeError("Filters are not allowed in somple expressions!",true);
            return NULL;
        }
		// check for zero length stuff
		if (xsrc.length()<1) {
            MakeError("The filter operand is badly formed!",true);
            return NULL;
		}
		// looking for the splitter
        semiloc = FindNonEscaped(xsrc,";",0);
		// local vs global
        if (semiloc<0) {
			// local
            UnEscape(xsrc,";");
			res = new jfLocalFilterElem(localmap,xsrc);
		}
		else {
			// global
			SplitString(xsrc,semiloc,fpart,spart);
            if (fpart.isEmpty() || spart.isEmpty()) {
                MakeError("The global filter reference is badly formed!",true);
                return NULL;
			}
            UnEscape(fpart,";");
            UnEscape(spart,";");
			// making the filter
			res = new jfGlobalFilterElem(fpart,spart);
		}
	}

	// it is actually a string operand...
	else {
		// we look for a split...
        semiloc = FindNonEscaped(xsrc,";",0);
		// if there is no split, we treat it a a case insensetive non-title substring
        if (semiloc < 0) res = new jfSubstringElem(xsrc);
		// otherwise...
		else {
			// we split things into two parts
			slen = xsrc.length();
            fpart = xsrc.mid(0,semiloc);
            fpart = fpart.trimmed();
            spart = xsrc.mid(semiloc+1);
            UnEscape(spart,";");
            spart = spart.trimmed();
			// running a quick error check
            if (spart.isEmpty()) {
                MakeError("The string operand is empty!",true);
                return NULL;
			}

            jfExpMatchType mtype;
            if (!CheckId(fpart,mtype,iscs,istit)) {
                MakeError("The id string is invalid!",true);
				return NULL;
			}
			// titles are not in simple expressions
			if (simple && istit) {
                MakeError("Simple expressions do not have title strings",true);
                return NULL;
			}
			// everthing is ok, we now create...
            if (mtype == jfemt_TOKENIZED) res = new jfTokenMElem(spart,iscs,istit);
            else if (mtype == jfemt_WHOLESTRING) res = new jfWholeStringElem(spart, iscs);
            else res = new jfSubstringElem(spart,iscs,istit);
		}
	}
	return res;
}
//-------------------------------------------------------
bool jfExpParserClass::FilterTest(QString& sourcef, QChar delimt) const {
    uint mlen;
	bool arroba;
	// running some basic checks
	mlen = sourcef.length();
	assert(mlen>0);
	if (mlen<2) return false;
	// we now see if things are a filter
	arroba = (sourcef[0]==delimt);
	if (!arroba) return false;
    arroba = (sourcef[mlen-1]==delimt);
	if (!arroba) return false;
	// a final check...
	if (mlen>=3) {
    if (sourcef[mlen-2]=='\\') {
      sourcef = sourcef.mid(0,mlen-1);
			sourcef[mlen-2] = delimt;
			return false;
		}
	}
	// we now assume things are a filter, so we trim the '@''s off
  sourcef = sourcef.mid(1,mlen-2);
  sourcef = sourcef.trimmed();
	// done
	return true;
}

//-------------------------------------------------------------------------
// deciphers the id that can be placed in fron of string tokens
bool jfExpParserClass::CheckId(const QString& idin, jfExpMatchType& ssout, bool& csout, bool& titout) const {
    // constants
    const QString char_set = "[" + v_stype + "]";
    const QString char_notset = "[^" + v_idchars + "]";
    const QRegExp re_char_set(char_set);
    const QRegExp re_char_notset(char_notset);

	// local variables
    QChar xchar,xchar2;
    int spos;
	size_t idlen = idin.length();
	bool chkcs = false;

	// a basic check
	if (idlen>2) return false;
	// checking for invalid chars
    if (idin.contains(re_char_notset)) return false;

	// we now move on to a special case: zero length id
	if (idlen==0) {
        ssout = jfemt_SUBSTRING; // for ordinary string processing
		csout = true;
		titout = false;
		return true;
	}
	// with that out of the way, we look for the type char
    spos = idin.indexOf(re_char_set);

	// case no type id found
    if (spos<0) {
		// the id must be length one, ( 'c' or 'i' only )
		if (idlen!=1) return false;
		chkcs = true;
		xchar2 = idin[0];
		// defualts
        ssout = jfemt_SUBSTRING;
		titout = false;
	}
	// a type id is found, we extract
	else {
		xchar = idin[spos];
		// we prepare for checking the other char, if needed
		if (idlen==2) {
			xchar2 = idin[(spos+1)%2];
			chkcs = true;
		}
		// testing what we extracted
        if (xchar=='s')      { ssout = jfemt_SUBSTRING; titout = false; }
        else if (xchar=='m') { ssout = jfemt_TOKENIZED; titout = false; }
        else if (xchar=='t') { ssout = jfemt_SUBSTRING; titout = true; }
        else if (xchar=='z') { ssout = jfemt_WHOLESTRING; titout = false; }
        else if (xchar=='q') { ssout = jfemt_TOKENIZED; titout = true; }
        else {
            // treat non-title substring matching as the default
            ssout = jfemt_SUBSTRING; titout = false;
        }
	}
	// moving on to case sensetive processing
	if (chkcs) {
    if (xchar2=='c') csout = true;
    else if (xchar2=='i') csout = false;
		else return false;
	}
	else csout = false;
	// done
	return true;

}
//=============================================================================
// following function is designed to take in an element array on produce a postfix
// expression verion
jfElemArray* MakeExprPostfix(jfElemArray* infixsrc, QString& outerr){
	// local variables
	jfElemArray* theresult;
	jfExprElem* tcopy;
	size_t src_count;
    QChar tchar,pchar;
	size_t tprec, pprec;
	jfOperatorStack* opstack;
	bool opnot, stop;
    QString fname1 = "MakeExprPostfix(jfElemArray* infixsrc)";

	// starting....
	assert(infixsrc!=NULL);
	src_count = infixsrc->size();

	// a basic check
	if (src_count==0) return new jfElemArray();
	// setting up for the loop
	opstack = new jfOperatorStack();
	theresult = new jfElemArray();

	// the evaluation loop
	for (size_t eindex = 0; eindex<src_count; eindex++) {
		// here we look for operators and parens
		if (infixsrc->NotOperand(eindex)) {
            // getting the type char
            tchar = ((*infixsrc)[eindex])->GetType();
            // lparens are puched on the stack
            if (tchar=='(') opstack->push(tchar);
            // rparens, we pop and write everything until a lparen is reached
            else if (tchar==')') {
				while (opstack->NotLParen()) {
					// a major error
					if (opstack->empty()) {
						delete opstack;
						delete theresult;
                        outerr = "EXPRESSION ERROR in " + fname1 + " : ";
                        outerr += "Mismatched parenthesis!";
						return NULL;
					}
					// ok!
					theresult->Append(opstack->PopElem());
				}
				// we discard what is left
				opstack->pop();
			}
			// we have encountered an operator, ~ is treated differently
			else {
				// basic checking
                opnot = (tchar=='~');
				tprec = GetPrec(tchar);
				// the popping loop
				while (opstack->IsOp()) {
					// checking the top of the stack
					pchar = opstack->top();
					pprec = GetPrec(pchar);
					// seeing if we stop popping and push
					if (opnot) stop = (pprec<=tprec);
					else stop = (pprec<tprec);
					// if we don't stop, we pop and write the operator
					if (!stop) {
						theresult->Append(opstack->PopElem());
					}
					else break;
				}
				// pushing the operand
				opstack->push(tchar);
			}
		}
		// we've got operands!
		else {
			// we just copy them over
			tcopy = CopyElement((*infixsrc)[eindex]);
			theresult->Append(tcopy);
		}
	}
	// we're at the end of the scanning loop
	// we now pop anything left on the stack
	while (opstack->IsOp()) {
		theresult->Append(opstack->PopElem());
	}
	// checking for errors
	if (opstack->IsLParen()) {
		delete opstack;
		delete theresult;
    outerr = "EXPRESSION ERROR in " + fname1 + " : ";
    outerr += "Mismatched parenthesis!";
    return NULL;
	}
	// clearing up
	delete opstack;
	return theresult;
}
//*****************************************************************************
