/*****************************************************************************
 Name:        misc_types1.cpp
 Author :     John Q Metro
 Purpose :    Some types
 Created:     April 2, 1010
 Conversion to Qt Started March 11, 2013
 Updated:     December 31, 2017
 ******************************************************************************/
#ifndef JFMISCTYPES1
	#include "misc_types1.h"
#endif
#ifndef UTILS1_H_INCLUDED
#include "../utils/utils1.h"
#endif // UTILS1_H_INCLUDED
#ifndef UTILS2_H_INCLUDED
#include "../utils/utils2.h"
#endif // UTILS2_H_INCLUDED
#ifndef LOGGING_H_INCLUDED
  #include "../utils/logging.h"
#endif // LOGGING_H_INCLUDED

#include <assert.h>
#include <vector>
//*****************************************************************************
QString jfTagListing::ToString() const {
    // local variables
    QString result,buf;
	jfTagListing::const_iterator indexer;
	// we start...
	if (!empty()) {
		// the loop
		for (indexer = begin(); indexer != end(); indexer++) {
			buf = EscapeString(indexer->first,IO_DELIMS);
            result += buf + ";";
            result += QString::number((size_t)(indexer->second));
            result += ";";
		}
		// removing the trailing ;
        result.chop(1);
	}
	return result;
}
//--------------------------------------------------------------------
bool jfTagListing::FromString(const QString& instr) {
  // constants
  const QString fname = "jfTagListing::FromString";
	// local variables
	jfLineParse* parser;
	size_t numtags, modc,ncount;
  QString intag;
	size_t incat;
	jfTAG_STATUS intags;
	// starting
	clear();
	// a special case...
  if (instr.isEmpty()) return true;
	// we use the parser
	parser = new jfLineParse(instr);
	modc = (parser->Num()) % 2;
	if (modc!=0) {
		delete parser;
		return false;
	}
	// things seem to be okay
	numtags = (parser->Num())/2;
	// the loop
	for (ncount = 0; ncount <numtags; ncount++ ) {
		// get and check the tag string
		intag = parser->UnEscStr(ncount*2);
        if (intag.isEmpty()) {
			delete parser;
			return false;
		}
		// get and convert the tag category
		if (!parser->SIntVal(ncount*2+1,incat)) {
			delete parser;
			return false;
		}
		intags = (jfTAG_STATUS)incat;
		// inserting
		(*this)[intag] = intags;
	}
	delete parser;
	return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// initializing
//----------------------------------------

bool jfTagListing::FromArray(const QString insrc[], size_t incount) {
  // basic check
  if (incount==0) return false;
  // variables
  size_t qloop;
  QString ctag;
  // main loop
  for (qloop=0;qloop<incount;qloop++) {
    ctag = QString(insrc[qloop]);
    if (ctag.isEmpty()) return false;
    (*this)[ctag] = jfts_NONE;
  }
  return true;
}
//---------------------------------------
bool jfTagListing::FromStrArray(const QStringList* insrc) {
  // checks
  if (insrc==NULL) return false;
  if (insrc->isEmpty()) return false;
  // variables
  size_t qloop,qmax;
  QString ctag;
  // start
  qmax = insrc->size();
  // main loop
  for (qloop=0;qloop<qmax;qloop++) {
    ctag = insrc->at(qloop);
    if (ctag.isEmpty()) return false;
    (*this)[ctag] = jfts_NONE;
  }
  return true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// special
bool jfTagListing::Valset() const {
  if (empty()) return false;
  // variables
  jfTagListing::const_iterator it;
  // checking
  it = begin();
  while (it!=end()) {
    if ((*it).second!=jfts_NONE) return true;
    it++;
  }
  return false;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// tag replacement
//-----------------------------------------------------
bool jfTagListing::ReplaceTag(QString oldt, QString newt) {
    // variables
    jfTagListing::iterator tpointer;
    jfTAG_STATUS temp;
    // special case
    if (oldt==newt) return false;
    // getting the old value
    tpointer = find(oldt);
    if (tpointer==end()) return false;
    temp = tpointer->second;
    // deleting and reinserting
    erase(tpointer);
    (*this)[newt] = temp;
    return true;
}
//--------------------------------------------------------------------
bool jfTagListing::ReplaceTags(QStringList* replist) {
    // bad numbers of items
    if (replist==NULL) return false;
    if (replist->isEmpty()) return false;
    if ((replist->size())&1) return false;
    // replacement loop
    size_t rcount = 0;
    size_t nindex;
    size_t nmax = (size_t)((replist->size()) / 2);
    for (nindex = 0; nindex < nmax; nindex++) {
        if (ReplaceTag(replist->at(2*nindex),replist->at(2*nindex+1))) rcount++;
    }
    // done
    return (rcount==nmax);

}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// special, returns true if the tags match the given list
bool jfTagListing::MatchTags(const char* inlist[], size_t inlistcount) {
  // basic check
  if (inlistcount!=size()) return false;
  // variables
  QString ctag;
  size_t lindex;
  // the main loop
  for (lindex=0;lindex<inlistcount;lindex++) {
    ctag = QString(inlist[lindex]);
    if (find(ctag)==end()) return false;
  }
  return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/* special, compares inputted tags to tags already in the list. Returns FALSE if
there are tags in the list not in input. Tags in input that are not in the list
are added. falsechoice controls what we do if we return false. options are
 0:ignore, 1:remove, and 2:abort */
bool jfTagListing::Reconcile(const char* inlist[], size_t inlistcount, size_t falsechoice) {
  // basic check
  assert(inlistcount==size());
  assert(falsechoice<3);
  // variables
  size_t inccount = 0;
  size_t incindex;
  std::vector<bool> inc_list;
  iterator riter;
  bool resval = true;
  bool found = false;
  // checking the input against what is on the list
  for (incindex=0;incindex<inlistcount;incindex++) {
    if (find(QString(inlist[incindex]))!=end()) {
      inc_list.push_back(true);
      inccount++;
    }
    else inc_list.push_back(false);
  }
  // with the results...
  if (inccount<size()) {
    if (falsechoice==2) return false;
    else resval = false;
  }
  // removing things can be pretty complicated
  if (falsechoice==1) {
    riter = begin();
    while (riter!=end()) {
      for (incindex=0;incindex<inlistcount;incindex++) {
        found = ((riter->first)==QString(inlist[incindex]));
        if (found) break;
      }
      if (!found) erase(riter++);
      else riter++;
    }
  }
  // by contrast, adding theing is simple
  for (incindex=0;incindex<inlistcount;incindex++) {
    if (!inc_list[incindex]) (*this)[inlist[incindex]] = jfts_NONE;
  }
  // done
  return resval;
}
//===========================================================================
QString UpdateStatus2String(const jf_FUpdateStatus& inval) {
  QString result;
  if (inval==jud_NONE) result.clear();
  else if (inval==jud_NEW) result = "New!";
  else if (inval==jud_UPDATED) result = "Updated!";
  else if (inval==jud_MISSING) result = "Missing!";
  else if (inval==jud_AUTHORNC) result = "Author Name Changed!";
  else assert(false);
  // done
  return result;
}
//==========================================================================
bool QStringSet::contains(const QString& test) const {
    return (count(test)>0);
}
//*****************************************************************************


