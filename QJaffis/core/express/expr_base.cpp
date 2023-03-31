/////////////////////////////////////////////////////////////////////////////
// Name:        expr_base.cpp
// Author :     John Q Metro
// Purpose :    Defines some expression element classes
// Created:     26.12.06
// Conversion to QT Started April 8, 2013
// Updated:     August 6, 2022 (added jfWholeStringElem)
/////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------
#ifndef JFEXPRBASE
#include "expr_base.h"
#endif
#ifndef JFILTERGLOBALS
#include "../filters/filter_globals.h"
#endif
#ifndef EXPRESSION_H
  #include "../filters/base/expression.h"
#endif // EXPRESSION_H
#ifndef UTILS1_H_INCLUDED
#include "../utils/utils1.h"
#endif // UTILS1_H_INCLUDED
#ifndef UTILS3_H_INCLUDED
  #include "../utils/utils3.h"
#endif // UTILS3_H_INCLUDED
#ifndef LOGGING_H_INCLUDED
  #include "../utils/logging.h"
#endif // LOGGING_H_INCLUDED

#include <assert.h>
//****************************************************************************
const QString expr_operators = "&|%~";
const QString expr_operands = "smtqzlge";
const QString expr_scitems = expr_operators + "()";
const QString expr_precstring = "&3|3%3~5";
//****************************************************************************
// a special function for copying an element
jfExprElem* CopyElement(jfExprElem* source) {
	// global variables
	jfExprElem *rtype;
	jfOperandElem* tempo;
    QChar gtype;
	// basic checking...
	assert(source!=NULL);
	// the main two way divisiton
	if (source->IsOperand()) {
		tempo = dynamic_cast<jfOperandElem*>(source);
		gtype = tempo->GetType();
		// another two way division
		if (tempo->IsString()) {
			// handling duplication of string elements
            if ((gtype=='s') || (gtype=='t')) {
				rtype = new jfSubstringElem(*(dynamic_cast<jfSubstringElem*>(source)));
			}
            else if ((gtype=='m') || (gtype=='q')) {
				rtype = new jfTokenMElem(*(dynamic_cast<jfTokenMElem*>(source)));
			}
            else if (gtype=='z') {
                rtype = new jfWholeStringElem(*(dynamic_cast<jfWholeStringElem*>(source)));
            }
			else assert(false);
		}
		else {
			// handling duplication of filter elements
            if (gtype == 'l') {
				rtype = new jfLocalFilterElem(*(dynamic_cast<jfLocalFilterElem*>(source)));
			}
            else if (gtype == 'g') {
				rtype = new jfGlobalFilterElem(*(dynamic_cast<jfGlobalFilterElem*>(source)));
			}
			else {
				rtype = new jfLiteralFilterElem(*(dynamic_cast<jfLiteralFilterElem*>(source)));
			}
		}
	}
	else {
		// producing a new copy
		rtype = new jfOperatorElem(*(dynamic_cast<jfOperatorElem*>(source)));
	}
	// done
	return rtype;
}
//============================================================================
// getting precedence
size_t GetPrec(const QChar& intype) {
	// local variables
    QString xchar;
    int xres;
    unsigned long xval;
	// looking
    xres = expr_precstring.indexOf(intype,0);
	// checking the result...
    assert(xres>=0);
	assert((xres%2)==0);
	// now we get the precedence char
	xchar = expr_precstring[xres+1];
	// converting...
    if (Str2ULong(xchar,xval)) return xval;
	// the conversion has failed...
	else {
		assert(false);
		return 0;
	}
}
//============================================================================
/* Name Verification is used on elements that have links to external filters, or
 may have them. Two possible errors can arise: one, the referred filters may not
 exist, two, the filters may refer to themselves, directly or indirectly. NameVerif
 is a class used in verification to avoid this */
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the default constructor
jfNameVerif::jfNameVerif() {
	local_list=NULL;
	local_count=0;
	global_list=NULL;
	global_count=0;
	inerror=false;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// some special error handling methods
//--------------------------------------------------------------------------
// the chainstring is listing of filters that lead up to this point
bool jfNameVerif::MakeChainString() {
	// variables
	size_t lcounter, gcounter;
  QString rtotal, cbuf;
	// we loop thru local list
	for (lcounter=0;lcounter<local_count;lcounter++) {
    cbuf = local_list->at(lcounter);
		rtotal += cbuf;
    if ((lcounter-1)!=local_count) rtotal += "->";
	}
	// adding in a -> if need be
  if ((local_count!=0) && (global_count!=0)) rtotal += "->";
	// we loop thru global list
	for (gcounter=0;gcounter<global_count;gcounter++) {
		// mapname first
    cbuf = global_list->at(2*gcounter);
    rtotal += cbuf + ';';
		// filter name second
    cbuf = global_list->at((2*gcounter)+1);
		// arrow if need be
    if ((gcounter-1)!=global_count) rtotal += "->";
	}
	// almost done
	chainstring = rtotal;
  return !rtotal.isEmpty();
}
//--------------------------------------------------------------------------
QString jfNameVerif::GetChainString() const {
	return chainstring;
}
//--------------------------------------------------------------------------
bool jfNameVerif::InGlobal() const {
	return (global_count!=0);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// looking for names
//--------------------------------------------------------------------------
// looks thru the list of local filters for names
bool jfNameVerif::FindName(const QString& fname) {
	// variables
	size_t lcounter;
	bool found=false;
  QString cbuf;
	// we loop thru local list
	for (lcounter=0;lcounter<local_count;lcounter++) {
    cbuf = local_list->at(lcounter);
		found = (cbuf==fname);
		if (found) break;
	}
	return found;
}
//--------------------------------------------------------------------------
// looking thru the list of global filters for names
bool jfNameVerif::FindName(const QString& mapname, const QString& fname) {
  // constants
  const QString fxname = "jfNameVerif::FindName";
	// variables
	size_t gcounter;
	bool found=false;
  QString cbuf;
	// checks
  /**/JDEBUGLOGST(fxname,1,global_count)
  /**/JDEBUGLOGB(fxname,2,global_list!=NULL)
  /**/JDEBUGLOGS(fxname,4,mapname)
	// we loop thru global list
	for (gcounter=0;gcounter<global_count;gcounter++) {
		// checking for mapname equivalency
    cbuf = global_list->at(2*gcounter);
    /**/JDEBUGLOGS(fxname,5,cbuf)
		found = (cbuf==mapname);
		if (found) {
			// we now check the filter name
      cbuf = global_list->at(1+(2*gcounter));
      /**/JDEBUGLOGS(fxname,6,cbuf)
			found = (cbuf==fname);
		}
		if (found) break;
	}
  /**/JDEBUGLOG(fxname,7);
	return found;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// name push and pop
//--------------------------------------------------------------------------
// pushes an item on the global list
void jfNameVerif::PushGlobal(const QString& mapname, const QString& fname) {
  if (global_list==NULL) global_list = new QStringList;
  global_list->append(mapname);
  global_list->append(fname);
	global_count++;
}
//--------------------------------------------------------------------------
// pushes an item onto the local list
bool jfNameVerif::PushLocal(const QString& fname) {
	if (InGlobal()) return false;
  if (local_list==NULL) local_list = new QStringList;
  local_list->append(fname);
	local_count++;
	return true;
}
//--------------------------------------------------------------------------
/* pops a name off the list. given that global follows local, we only pop
 off the local list once the is global is exhaused */
bool jfNameVerif::PopName() {
	// removing from the global list
	if (InGlobal()) {
    global_list->removeAt(2*(global_count-1));
    global_list->removeAt(2*(global_count-1));
		global_count--;
		if (global_count==0) {
			delete global_list;
			global_list=NULL;
		}
		return true;
	}
	// removing a name from the local list
	else if (local_count!=0) {
    local_list->removeAt(local_count-1);
		local_count--;
		if (local_count==0) {
			delete local_list;
			local_list = NULL;
		}
		return true;
	}
	// truying to pop from a completely empty list
	else return false;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// setting and getting the error
//--------------------------------------------------------------------------
bool jfNameVerif::InError() const {
	return inerror;
}
//--------------------------------------------------------------------------
void jfNameVerif::SetLocalError(const QString& lfiltname) {
  errormsg = "Error: local filter reference \"" + lfiltname;
  errormsg += "\" from global filter:\n";
	MakeChainString();
	errormsg += chainstring;
	inerror = true;
}
//--------------------------------------------------------------------------
void jfNameVerif::SetNotFoundError(const QString& fname) {
  errormsg = "Error: Local referenced filter \"" + fname;
  errormsg += "\" is not found.";
	MakeChainString();
  if (!chainstring.isEmpty()) {
    errormsg += " Referenced from:\n";
		errormsg += chainstring;
	}
	inerror = true;
}
//--------------------------------------------------------------------------
void jfNameVerif::SetNotFoundError(const QString& mapname, const QString& fname) {
  errormsg = "Error: Global referenced filter \"" + mapname + ';' + fname;
  errormsg += "\" is not found.";
	MakeChainString();
  if (!chainstring.isEmpty()) {
    errormsg += " Referenced from:\n";
		errormsg += chainstring;
	}
	inerror = true;
}
//--------------------------------------------------------------------------
void jfNameVerif::SetCircularError(const QString& fname) {
  errormsg = "Error: Circular reference. Filter \"" + fname;
  errormsg += "\" Referenced from:\n";
	errormsg += chainstring;
	inerror = true;
}
//--------------------------------------------------------------------------
void jfNameVerif::SetCircularError(const QString& mapname, const QString& fname) {
  errormsg = "Error: Circular reference. Filter \"" + mapname;
  errormsg += ';' + fname;
  errormsg += "\" Referenced from:\n";
	errormsg += chainstring;
	inerror = true;
}
//--------------------------------------------------------------------------
QString jfNameVerif::GetError() const {
	return errormsg;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the destructor
jfNameVerif::~jfNameVerif() {
	if (global_list!=NULL) delete global_list;
	if (local_list!=NULL) delete local_list;
}
//============================================================================
QChar jfExprElem::GetType() const { return type; }
jfExprElem::~jfExprElem() { }
//============================================================================
// the constructor for operators
jfOperatorElem::jfOperatorElem(QChar typestr) {
  assert(expr_scitems.contains(typestr));
	type = typestr;
}
//---------------------------------------------------------------------------
jfOperatorElem::jfOperatorElem(const jfOperatorElem& source) {
	type = source.type;
}
//---------------------------------------------------------------------------
bool jfOperatorElem::IsOperand() const { return false; }
//--------------------------------------------------------------------------
QString jfOperatorElem::String() const {
  QString res;
  res = "<";
  if (type=='&') res+= "&";
  res += QString(type) + ">";
	return res;
}
//===========================================================================
// the following two methods are pretty simple
bool jfOperandElem::IsOperand() const { return true; }
//---------------------------------------------------------------------------
bool jfOperandElem::IsValid() const { return valid; }
//===========================================================================
// basic extensions for substring matching
QString jfStrElem::GetPattern() const { return pattern; }
//---------------------------------------------------------------------------
bool jfStrElem::GetCasesen() const { return casesen; }
//---------------------------------------------------------------------------
bool jfStrElem::IsString() const { return true; }
//---------------------------------------------------------------------------
bool jfStrElem::GetTitle() const { return title; }
//--------------------------------------------------------------------------
QString jfStrElem::String() const {
  QString res;
  res = "<";
  res+= QString(type) + ";";
  if (casesen) res+= 't';
  else res+= 'f';
  if (title) res+= 't';
  else res+= 'f';
  res+= ';' + pattern+">";
	return res;
}
//===========================================================================
// the substring matching element
// the constructor is below
jfSubstringElem::jfSubstringElem(const QString& newpattern, bool newcs,bool istitle):jfStrElem() {
	// loading the basics
  pattern = newpattern.trimmed();
  assert(!(pattern.isEmpty()));
	// setting the values
	casesen = newcs;
	valid = true;
  if (istitle) type = 't';
  else type = 's';
	title = istitle;
}
//---------------------------------------------------------------------------
// the copy constructor
jfSubstringElem::jfSubstringElem(const jfSubstringElem& source) {
	type = source.type;
	pattern = source.pattern;
	casesen = source.casesen;
	title = source.title;
	valid = source.valid;
}
//---------------------------------------------------------------------------
// substring matching
bool jfSubstringElem::Match(const QString& base) const {
  QString newbase, newpattern;
	// special checking
  newbase = base.trimmed();
  if (newbase.isEmpty()) return false;
	// converting by case sensetivity
	if (!casesen) {
    newpattern = pattern.toLower();
    newbase = newbase.toLower();
	}
	else newpattern = pattern;
	// performing the check
  return (newbase.contains(newpattern));
}
//============================================================================
// token matching
//---------------------------------------------------------------
jfTokenMElem::jfTokenMElem(const QString& newpattern, bool newcs,bool istitle) {
	// loading the basics
  pattern = newpattern.trimmed();
  assert(!(pattern.isEmpty()));
	// setting the values
	casesen = newcs;
	valid = true;
  if (istitle) type = 'q';
  else type = 'm';
	title = istitle;
	// tokenizing..
	tdata = TokenizeString(pattern,false);
}
//---------------------------------------------------------------
// the copy constructor
jfTokenMElem::jfTokenMElem(const jfTokenMElem& source) {
	type = source.type;
	pattern = source.pattern;
	casesen = source.casesen;
	title = source.title;
	valid = source.valid;
	if (source.tdata == NULL) tdata = NULL;
  else tdata = new QStringList(*(source.tdata));
}
//---------------------------------------------------------------
// token matching, a bit different from substring matching...
bool jfTokenMElem::Match(const QStringList* base) const {
  // constants
  const QString fname = "jfTokenMElem::Match";
  // variables
  QString newbase,newpattern;
	size_t index1,index2;
	size_t countp,countb;
	bool found;
  Qt::CaseSensitivity csvalue;
  QRegExp rpattern;
	// basic checking...
  if (pattern=="*") return true;
	if (base==NULL) return false;
  countp = tdata->size();
  countb = base->size();
	if (countb<countp) return false;
  // preparing for the match
  csvalue = (casesen)?(Qt::CaseSensitive):(Qt::CaseInsensitive);
	// the loop
	for(index1=0;index1<(countb-(countp-1));index1++) {
		// getting the token
    newbase = base->at(index1);
    // building the pattern
    newpattern = tdata->at(0);
    rpattern = QRegExp(newpattern,csvalue,QRegExp::WildcardUnix);
    // checking to see if it matches the pattern
    if (rpattern.exactMatch(newbase)) {
			if (countp == 1) return true;
			// for multi-item pattern, we match more...
			found = true;
			for (index2=(index1+1);index2<(index1+countp);index2++) {
				// getting the token
        newbase = base->at(index2);
        // converting by case sensetivity...
        newpattern = tdata->at(index2-index1);
        rpattern = QRegExp(newpattern,csvalue,QRegExp::WildcardUnix);
				// matching
        if (!rpattern.exactMatch(newbase)) {
					found = false;
					break;
				}
			}
			// we've exited the inner loop, have we found anything?
			if (found) return true;
		}
	}
	return false;
}
//-------------------------------------------------------------------------
// the destructor
jfTokenMElem::~jfTokenMElem() {
	if (tdata!=NULL) {
		delete tdata;
		tdata = NULL;
	}
}
//===========================================================================
// the entire string matching element
// the constructor is below
jfWholeStringElem::jfWholeStringElem(const QString& newpattern, bool newcs) {
    // loading the basics
    pattern = newpattern.trimmed();
    assert(!(pattern.isEmpty()));
    // setting the values
    casesen = newcs;
    valid = true;
    title = false;
    type = 'z';
}
//---------------------------------------------------------------------------
// the copy constructor
jfWholeStringElem::jfWholeStringElem(const jfWholeStringElem& source) {
    type = source.type;
    pattern = source.pattern;
    casesen = source.casesen;
    title = source.title;
    valid = source.valid;
}
//---------------------------------------------------------------------------
// entire string matching
bool jfWholeStringElem::Match(const QString& base) const {
    QString newbase, newpattern;
    // special checking
    newbase = base.trimmed();
    if (newbase.isEmpty()) return false;
    // converting by case sensetivity
    if (!casesen) {
      newpattern = pattern.toLower();
      newbase = newbase.toLower();
    }
    else newpattern = pattern;
    // performing the check
    return (newbase == newpattern);
}
//==========================================================================
/* Operand class for embedded filters literals. We disallow embedded expression
 filters (they are redundant). This simplifies things quite a bit. */
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// constructors
//---------------------------------------------------------------------------
jfLiteralFilterElem::jfLiteralFilterElem(const QString& ftype, const QString& rawdata) {
	bool testx;
	// basic assignments
    type = 'e';
	valid = false;
	filttype = ftype;
	source = rawdata;
	thefilter = NULL;

	// checking
	assert(filttype!=EXP_FIL_TAG);
	// we try to create the filter
    thefilter = MakeFilter(ftype,"tempnamex1");
	if (thefilter!=NULL) {
		testx = thefilter->FromString(rawdata);
		if (!testx) {
			delete thefilter;
			valid = false;
			thefilter = NULL;
		}
		else valid = true;
	}
	// done
}
//---------------------------------------------------------------------------
jfLiteralFilterElem::jfLiteralFilterElem(const jfLiteralFilterElem& fsource) {
  type = 'e';
	valid = fsource.valid;
	filttype = fsource.filttype;
	source = fsource.source;
	thefilter = fsource.thefilter->GenCopy();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// public methods
bool jfLiteralFilterElem::Match(const jfSearchResultItem* testelem) const {
	// basic checks
	assert(valid);
	assert(testelem!=NULL);
	// the match
	return (thefilter->Match(testelem));
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// virtual methods that need to be defined
//---------------------------------------------------------------------------
QString jfLiteralFilterElem::String() const {
  QString res;
  res = '<';
  res += QString(type) + ';';
	res+= filttype;
  res += ';' + source + '>';
	return res;
}
//---------------------------------------------------------------------------
bool jfLiteralFilterElem::IsString() const {
	return false;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// destuctor
jfLiteralFilterElem::~jfLiteralFilterElem() {
	if (thefilter!=NULL) delete thefilter;
}
//==========================================================================
// a base class for embedded filter operands
//--------------------------------------------------------------------------
bool jfFilterElem::IsString() const { return false; }
//---------------------------------------------------------------
// does the matching
bool jfFilterElem::Match(const jfSearchResultItem* testelem) const {
	// basic checks
	assert(valid);
	assert(testelem!=NULL);
	// the match
	return (thefilter->Match(testelem));
}
//==========================================================================
// external refernced filter elem constructor !!!
jfGlobalFilterElem::jfGlobalFilterElem(const QString& inmapname, const QString& filtername) {
	// setting the basic values
  type = 'g';
	mapname = inmapname;
	filter_name = filtername;
	valid = false;
	thefilter = NULL;
	// when copying, we link the pointer as well...
	c_pointer = filterglobals::global_filters->GetPointer(mapname,filter_name);
}
//---------------------------------------------------------------------------
jfGlobalFilterElem::jfGlobalFilterElem(const jfGlobalFilterElem& source) {
	// copying the most basic stuff over
	type = source.type;
	// copying the strings
	mapname = source.mapname;
	filter_name = source.filter_name;
	// copying the filters...
	thefilter = NULL;
	// the pointer may no longer be actually valid...
	c_pointer = NULL;
	valid = false;
}
//----------------------------------------------------------------------
// tries to see if the filter linked to is an expression filter
bool jfGlobalFilterElem::IsExpr() const {
	if (c_pointer==NULL) return false;
	else {
		return ((c_pointer->filter)->MatchTypeid(EXP_FIL_TAG));
	}
}
//-------------------------------------------------------------------------
/* for externally referenced filters, this function is the most critical */
bool jfGlobalFilterElem::VerfyNames(jfNameVerif* nameobj){
  //
  const QString fname ="jfGlobalFilterElem::VerfyNames";
	// basic asserts
	assert(nameobj!=NULL);
	//local vars
	jfExpressionFilter* temp;
	bool resval;
	// first, we search for loops
  /**/JDEBUGLOGS(fname,1,mapname)
  /**/JDEBUGLOGS(fname,2,filter_name)
	resval = nameobj->FindName(mapname, filter_name);
	if (resval) {
		nameobj->SetCircularError(mapname,filter_name);
		valid = false;
    /**/JDEBUGLOG(fname,3)
		return false;
	}
	/* we'll leave pushing the current name to the expression filter, if the
	 filter we reference *is* an expression filter */
	// next, we try to link...
	if (c_pointer!=NULL) delete c_pointer;
	c_pointer = filterglobals::global_filters->GetPointer(mapname,filter_name);
  /**/JDEBUGLOGS(fname,4,mapname)
  /**/JDEBUGLOGS(fname,5,filter_name)
	// the link has failed, and the global filter is not found!
	if (c_pointer==NULL) {
		// error...
		valid = false;
    /**/JDEBUGLOG(fname,6)
		nameobj->SetNotFoundError(mapname,filter_name);
		return false;
	}
  /**/JDEBUGLOG(fname,7)
	// the link is a success!
	// we now check to see if what we see should be looked at more...
	if (IsExpr()) {
    /**/JDEBUGLOG(fname,8)
		temp = dynamic_cast<jfExpressionFilter*>(c_pointer->filter);
		// the filter is an expression, so we call its own VerifyNames
    /**/JDEBUGLOGS(fname,9,temp->ToString())
		resval = temp->VerifyNames(nameobj,mapname);
		valid = resval;
    /**/JDEBUGLOGB(fname,10,valid)
		return resval;
	}
	// the linked filter is not an expression, no problem
	else {
    /**/JDEBUGLOG(fname,11)
		valid = true;
		return true;
	}
}
//--------------------------------------------------------------------------
// to avoid problems with a filter changing while searching, the external
// filters are copied before any search is launch -- but not before
bool jfGlobalFilterElem::CopyLoad() {
	// to make sure names are validated first...
	assert(valid);
	assert(c_pointer!=NULL);
	// local variables
	jfExpressionFilter* temp;
	// going ahead and copying
	thefilter = (c_pointer->filter)->GenCopy();
	valid = true;
	// now, send the copying deeper
	if (IsExpr()) {
		temp = dynamic_cast<jfExpressionFilter*>(thefilter);
		if (temp->CopyLoad()) valid = true;
		else {
			// the following undoes copying at an element level
			delete thefilter;
			thefilter = NULL;
			valid = false;
		}
	}
	return valid;
}
//--------------------------------------------------------------------------
QString jfGlobalFilterElem::String() const {
  QString res;
  res = '<';
  res += QString(type) + ';';
	res+= mapname;
  res += ';' + filter_name + '>';
	return res;
}
//---------------------------------------------------------------------------
jfGlobalFilterElem::~jfGlobalFilterElem() {
	if (thefilter!=NULL) delete thefilter;
	if (c_pointer!=NULL) delete c_pointer;
}
//===========================================================================
// the constructor for the referenced filter
jfLocalFilterElem::jfLocalFilterElem(jfFilterMap* inlocalmap, const QString& filtername) {
	// starting
	assert(inlocalmap!=NULL);
	// settling the basic values
    type = 'l';
	filter_name = filtername;
	localmap = inlocalmap;
	thefilter = NULL;
	valid = false;
}
//---------------------------------------------------------------------------
// the copy constructor
jfLocalFilterElem::jfLocalFilterElem(const jfLocalFilterElem& source) {
	// copying the most basic stuff over
	type = source.type;
	valid = source.valid;
	filter_name = source.filter_name;
	localmap = source.localmap;
	thefilter = NULL;
}
//-------------------------------------------------------------------------
// checks if the referenced filter is an expression filter
bool jfLocalFilterElem::IsExpr() const {
	if (thefilter==NULL) return false;
	else {
		return (thefilter->MatchTypeid(EXP_FIL_TAG));
	}
}
//---------------------------------------------------------------------------
/* filter checking is similar to that for the global filter reference, with
 the extra bit checking that we do not go from global to local */
bool jfLocalFilterElem::VerfyNames(jfNameVerif* nameobj) {
  // constants
  const QString fname = "jfLocalFilterElem::VerfyNames";
	// basic asserts
	assert(nameobj!=NULL);
	//local vars
	jfExpressionFilter* temp;
	bool resval;
	// we check if the filter reference chain is global
  /**/JDEBUGLOG(fname,1)
	if (nameobj->InGlobal()) {
		nameobj->SetLocalError(filter_name);
		valid = false;
		return false;
	}
  /**/JDEBUGLOG(fname,2)
	// first, we search for loops
	resval = nameobj->FindName(filter_name);
	if (resval) {
		nameobj->SetCircularError(filter_name);
		valid = false;
		return false;
	}
  /**/JDEBUGLOG(fname,4)
	/* we'll leave pushing the current name to the expression filter, if the
	 filter we reference *is* an expression filter */
	// next, we try to link...
	if (thefilter!=NULL) delete thefilter;
	thefilter = localmap->GetItem(filter_name);
  /**/JDEBUGLOG(fname,5)
	// the link has failed, and the local filter is not found!
	if (thefilter==NULL) {
		// error...
		valid = false;
		nameobj->SetNotFoundError(filter_name);
		return false;
	}
  /**/JDEBUGLOG(fname,6)
	// the link is a success!
	// we now check to see if what we see should be looked at more...
	if (IsExpr()) {
		temp = dynamic_cast<jfExpressionFilter*>(thefilter);
		// the filter is an expression, so we call its own VerifyNames
		valid = temp->VerifyNames(nameobj,false);
	}
	// the linked filter is not an expression, no problem
	else valid = true;
  /**/JDEBUGLOGB(fname,7,valid)
	// finishing up
	thefilter = NULL;
	return valid;
}
//--------------------------------------------------------------------------
// to avoid problems with a filter changing while searching, the external
// filters are copied before any search is launch -- but not before
bool jfLocalFilterElem::CopyLoad() {
	// to make sure names are validated first...
	assert(valid);
	// local variables
	jfExpressionFilter* temp;
	// the basic copying
	thefilter = (localmap->GetItem(filter_name))->GenCopy();
	// let us check if we need to send the copying deeper...
	if (IsExpr()) {
		temp = dynamic_cast<jfExpressionFilter*>(thefilter);
		if (temp->CopyLoad()) valid = true;
		else {
			delete thefilter;
			thefilter = NULL;
			valid = false;
		}
	}
	else valid = true;
	return valid;
}
//----------------------------------------------------------------------
QString jfLocalFilterElem::String() const {
  QString res;
  res = '<';
  res += QString(type) + ';';
  res+= filter_name + '>';
	return res;
}
//---------------------------------------------------------------------------
jfLocalFilterElem::~jfLocalFilterElem() {
	if (thefilter!=NULL) delete thefilter;
}
//****************************************************************************
