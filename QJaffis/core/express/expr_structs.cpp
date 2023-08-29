/////////////////////////////////////////////////////////////////////////////
// Name:        expr_structs.cpp
// Author :     John Q Metro
// Purpose :    Defines basic structures for expressions
// Created:     30.12.06
// Conversion to QT Started April 8, 2013
// Updated:     March 29, 2009
/////////////////////////////////////////////////////////////////////////////
// headers
//----------------------------------------------------------------------------
#ifndef JFEXPRSTRUCTS
#include "expr_structs.h"
#endif
#ifndef UTILS1_H_INCLUDED
#include "../utils/utils1.h"
#endif // UTILS1_H_INCLUDED
#ifndef LOGGING_H_INCLUDED
  #include "../utils/logging.h"
#endif // LOGGING_H_INCLUDED

#include <assert.h>
//*****************************************************************************
// checks to see if the operator at the top is a left parenthesis
bool jfOperatorStack::IsLParen() const {
	if (empty()) return false;
  return (top()=='(');
}
//-------------------------------------------------------------------------
// checks to see if the operator at the top is *NOT* a left parenthesis
bool jfOperatorStack::NotLParen() const {
	if (empty()) return true;
  return (top()!='(');
}
//-------------------------------------------------------------------------
// checks to see if there is an actual operator, returns false id empty or
// there is a left parentehsis
bool jfOperatorStack::IsOp() const {
	if (empty()) return false;
  return (top()!='(');
}
//-------------------------------------------------------------------------
// the element is an operator, which we push on the stack. Since the type
// is the full content of an operator element, we only need the type
void jfOperatorStack::PushElem(const jfExprElem* operat) {
	// basic checks
	assert(operat!=NULL);
	assert(!(operat->IsOperand()));
	// pushing
	push(operat->GetType());
}
//-------------------------------------------------------------------------
// this version of the pop creates an element to wrap the operator char
jfExprElem* jfOperatorStack::PopElem() {
	jfOperatorElem* newres;
	if (empty()) return NULL;
	newres = new jfOperatorElem(top());
	pop();
	return newres;
}
//--------------------------------------------------------------------------
// combines top() and pop()
QChar jfOperatorStack::Get() {
  QChar xres;
  if (empty()) return '\0';
	xres = top();
	pop();
	return xres;
}
//============================================================================
// --- [ Methods for jfElemArray ] -------
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the constructors
//-------------------------------------------------------------------------
// the defualt contructor
jfElemArray::jfElemArray():std::vector<jfExprElem*>() {
	opcount = 0;
	tokcount = 0;
	ttokcount = 0;
}
//-------------------------------------------------------------------------
// the copy constructor
jfElemArray::jfElemArray(const jfElemArray& source):std::vector<jfExprElem*>() {
	opcount = 0;
	tokcount = 0;
	ttokcount = 0;
	// copying things over
	size_t xcount = source.size();
	reserve(xcount);
	// the copying loop
	for (size_t i = 0; i<xcount; i++) {
		Append(CopyElement(source[i]));
	}
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Adding and removing items
//-------------------------------------------------------------------------
size_t jfElemArray::Append(jfExprElem* newitem) {
  QChar titem;
	// checking for invalidity
	assert(newitem!=NULL);
	// adding
	push_back(newitem);
	// setting the tracking values
	if (!(newitem->IsOperand())) opcount++;
	else {
		titem = newitem->GetType();
    if (titem=='m') tokcount++;
    else if (titem=='q') ttokcount++;
	}
	// done
	return size();
}
//-------------------------------------------------------------------------
size_t jfElemArray::Append(const QChar& newitem) {
	jfOperatorElem* newwlwm;
	// we first check to see if the char is of the right type
  assert(expr_scitems.contains(newitem));
	// if it passes, we create and append...
	newwlwm = new jfOperatorElem(newitem);
	Append(newwlwm);
	return size();
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// basic info
//-------------------------------------------------------------------------
size_t jfElemArray::GetOpCount() const { return opcount; }
//-------------------------------------------------------------------------
size_t jfElemArray::GetTokCount() const { return tokcount; }
//-------------------------------------------------------------------------
size_t jfElemArray::GetTTokCount() const { return ttokcount; }
//-------------------------------------------------------------------------
// returns true if the element at the index is not an operand
bool jfElemArray::NotOperand(const size_t& index) const {
	assert(index<size());
	return (!( ( (*this)[index] )->IsOperand() ));
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// runs verify names on each of the elements it can
bool jfElemArray::VerifyNames(jfNameVerif* nameobj) const {
  // constants
  const QString fname = "jfElemArray::VerifyNames";
	// local variables
	bool result;
  QChar tchar;
	jfFilterElem* telem;
	// starting...
	assert(nameobj!=NULL);
	result = true;
	// the loop
	for (size_t index=0;index<size();index++) {
		// getting the type
		tchar = (*this)[index]->GetType();
    if ((tchar=='l') || (tchar=='g')) {
			// we run verify names on each sub-element if it is the right type
			telem = dynamic_cast<jfFilterElem*>((*this)[index]);
			result = telem->VerfyNames(nameobj);
			/**/if (!result) {
			  nameobj->MakeChainString();
        /**/JDEBUGLOGS(fname,1, telem->String())
        /**/JDEBUGLOGS(fname,2, nameobj->GetChainString())
			}
			if (!result) break;
		}
	}
	// done
	return result;
}
//-------------------------------------------------------------------------
bool jfElemArray::CopyLoad() {
	// local variables
	bool result;
  QChar tchar;
	jfFilterElem* telem;
	// starting...
	result = true;
	// the loop
	for (size_t index=0;index<size();index++) {
		// getting the type
		tchar = (*this)[index]->GetType();
    if ((tchar=='l') || (tchar=='g')) {
			// we run verify names on each sub-element if it is the right type
			telem = dynamic_cast<jfFilterElem*>((*this)[index]);
			result = telem->CopyLoad();
			if (!result) break;
		}
	}
	// done
	return result;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// clearing stuff
size_t jfElemArray::DeleteAll() {
	size_t fsize;
	fsize = size();
	for (size_t i = 0; i<fsize ; i++) delete (*this)[i];
	clear();
	return fsize;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// copying
jfElemArray* jfElemArray::Copy() const {
	return new jfElemArray(*this);
}
//============================================================================
// --- [ Methods for jfExprToker ] -------
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfExprToker::jfExprToker(const QString& sourceexpr) {
	// getting the source
  source = sourceexpr.trimmed();
	// setting some other stuff
	sleng = source.length();
	pastend = (sleng==0);
	index = 0;
	lindex = 0;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// getting stuff
//-------------------------------------------------------------------------
// gets the next token
QString jfExprToker::GetToken() {
    QString result;
    // the basic check
    assert(!pastend);
    // looking for the next delimiter
    int nextdelim = FindNonEscaped(source,expr_scitems,index);
    // checking...
    if (nextdelim == 0) {
        result = source[(uint)index];
        lindex = index;
        index++;
        pastend = (index==sleng);
    }
    // extract up to the next delimiter
    else if (nextdelim>=0) {
        // producing the result
        result = source.mid(index,nextdelim-index);
        result = result.trimmed();
        UnEscape(result,expr_scitems);
        // setting thingd
        lindex = index;
        index = nextdelim;
    }
    // we have not found any delimiter, we return everything left
    else {
        // extracting..
        result = source.mid(index);
        result = result.trimmed();
        lindex = index;
        index = sleng;
        pastend = true;
    }
    // done
    return result;
}
//-------------------------------------------------------------------------
// returns the next character, iff AtOp is true
QChar jfExprToker::GetTChar() {
	assert(AtOp());
	// we get the char
  QChar result = source[(uint)index];
	// incrementing the index
	lindex = index;
	index++;
	pastend = (index==sleng);
	// done
	return result;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// basic info
//-------------------------------------------------------------------------
bool jfExprToker::TokensLeft() const { return (!pastend); }
//-------------------------------------------------------------------------
// returns true if the first character at the index is an operator or ()
bool jfExprToker::AtOp() const {
	if (pastend) return false;
	// getting the character
  QChar tchar = source[(uint)index];
  return (expr_scitems.contains(tchar));
}
//-------------------------------------------------------------------------
// returns the index
size_t jfExprToker::GetIndex() const { return index; }
//-------------------------------------------------------------------------
size_t jfExprToker::LastIndex() const { return lindex; }
//============================================================================
// logical operators
//-------------------------------------------------------------------------
void jfTruthStack::And() {
	bool fir_op,sec_op;
	// basic checking
	assert(size()>1);
	// getting the operands
	sec_op = top();
	pop();
	fir_op = top();
	pop();
	// producing the result
	push(fir_op && sec_op);
}
//-------------------------------------------------------------------------
void jfTruthStack::Or() {
	bool fir_op,sec_op;
	// basic checking
	assert(size()>1);
	// getting the operands
	sec_op = top();
	pop();
	fir_op = top();
	pop();
	// producing the result
	push(fir_op || sec_op);
}
//-------------------------------------------------------------------------
void jfTruthStack::Xor() {
	bool fir_op,sec_op;
	// basic checking
	assert(size()>1);
	// getting the operands
	sec_op = top();
	pop();
	fir_op = top();
	pop();
	// producing the result
	push(fir_op && sec_op);
}
//-------------------------------------------------------------------------
void jfTruthStack::Not() {
	assert(!empty());
	bool tval = top();
	pop();
	push(!tval);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// special methods
bool jfTruthStack::One() const {
	return (size()==1);
}
//*****************************************************************************
