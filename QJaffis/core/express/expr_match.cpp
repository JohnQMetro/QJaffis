/////////////////////////////////////////////////////////////////////////////
// Name:        expr_match.cpp
// Author :     John Q Metro
// Purpose :    Defines functions that use the expression matches
// Created:     April 1, 2010
// Conversion to QT Started April 8, 2013
// Updated:     August 6, 2022 (added while string matching)
/////////////////////////////////////////////////////////////////////////////
#ifndef JFEXPRMATCH
	#include "expr_match.h"
#endif
#ifndef UTILS3_H_INCLUDED
  #include "../utils/utils3.h"
#endif // UTILS3_H_INCLUDED

#include <assert.h>
//**************************************************************************
// a matching function that uses everything: title, embedded filters, the works
bool FullItemExprMatch(const jfBasePD* testelem, const jfElemArray* parsedinfo) {
  // constants
  const QString fname = "FullItemExprMatch";
	// the default checks
  assert(testelem!=NULL);
	assert(parsedinfo!=NULL);
  // local variables
  jfTruthStack* tstack;
  jfOperandElem* topnd;
  jfFilterElem* tfilte;
  jfSubstringElem* t_string;
  jfTokenMElem* t_token;
  jfWholeStringElem* t_wholestring;
  jfLiteralFilterElem* lf_token;
  QChar theop;
  size_t eindex;
  bool tresult;
  bool fresult;

  // setting things up for evaluation
  tstack = new jfTruthStack();
  // evaluation
  for (eindex=0;eindex<(parsedinfo->size());eindex++) {
    // if things are an operand, we evaluate and push
    if ((*parsedinfo)[eindex]->IsOperand()) {
      topnd = dynamic_cast<jfOperandElem*>((*parsedinfo)[eindex]);
      // evaluating strings
      if (topnd->IsString()) {
        theop = topnd->GetType();
        // substring evaluation
        if ((theop=='s') || (theop=='t') ) {
          t_string = dynamic_cast<jfSubstringElem*>(topnd);
          if (t_string->GetTitle()) {
            tresult = t_string->Match(testelem->GetName());
          }
          else {
            tresult = t_string->Match(testelem->GetDescription());
          }

        }
        // tokenized evaluation
        else if ((theop=='m')||(theop=='q')) {
          t_token = dynamic_cast<jfTokenMElem*>(topnd);
          if (t_token->GetTitle()) {
            tresult = t_token->Match(testelem->GetTokTitle());
          }
          else {
            tresult = t_token->Match(testelem->GetTokSummary());
          }
        }
        // whole string evaluation
        else if (theop=='z') {
            t_wholestring = dynamic_cast<jfWholeStringElem*>(topnd);
            tresult = t_wholestring->Match(testelem->GetDescription());
        }
        else assert(false);

      }
      // evaluating an embedded filter
      else if ((topnd->GetType())=='e') {
        lf_token = dynamic_cast<jfLiteralFilterElem*>(topnd);
        tresult = lf_token->Match(testelem);
      }
      // evaluating linked filter operands
      else {
        tfilte = dynamic_cast<jfFilterElem*>(topnd);
        tresult = tfilte->Match(testelem);
      }
      tstack->push(tresult);
    }
    // otherwise, we do logical ops on the stack
    else {
      theop = (*parsedinfo)[eindex]->GetType();
      if (theop=='~') tstack->Not();
      else if (theop=='&') tstack->And();
      else if (theop=='|') tstack->Or();
      else if (theop=='%') tstack->Xor();
      else assert(false);
    }
  }
  // done, we now get the result and clear
  assert(tstack->One());
  fresult = tstack->top();
  delete tstack;
  testelem->ClearTok();
  // done
  return fresult;
}
//------------------------------------------------------------------------------
// a more limited matching function that works on strings only
bool StringExprMatch(const QString& teststr, const jfElemArray* parsedinfo) {
    // constants
    const QString fname = "@!!!!!! StringExprMatch";

    // the default checks
    assert(parsedinfo!=NULL);
    // local variables
    jfTruthStack* tstack;
    jfOperandElem* topnd;
    jfSubstringElem* t_string;
    jfTokenMElem* t_token;
    jfWholeStringElem* t_wholestring;
    QStringList* token_str;
    QChar theop;
    size_t eindex;
    bool tresult;
    bool fresult;

    // setting things up for evaluation
    tstack = new jfTruthStack();
    // evaluation
    for (eindex=0;eindex<(parsedinfo->size());eindex++) {

        // if things are an operand, we evaluate and push
        if ((*parsedinfo)[eindex]->IsOperand()) {
            topnd = dynamic_cast<jfOperandElem*>((*parsedinfo)[eindex]);
            // evaluating strings
            if (topnd->IsString()) {
                theop = topnd->GetType();
                // substring evaluation
                if (theop=='s')  {
                    t_string = dynamic_cast<jfSubstringElem*>(topnd);
                    tresult = t_string->Match(teststr);
                }
                // tokenized evaluation
                else if (theop=='m') {
                    t_token = dynamic_cast<jfTokenMElem*>(topnd);
                    token_str = TokenizeString(teststr,false);
                    tresult = t_token->Match(token_str);
                    delete token_str;
                }
                // whole string evaluation
                else if (theop=='z') {
                    t_wholestring = dynamic_cast<jfWholeStringElem*>(topnd);
                    tresult = t_wholestring->Match(teststr);
                }
                else assert(false);
            }
            // nothing other than strings allowed for this version
            else assert(false);
            tstack->push(tresult);
        }

        // otherwise, we do logical ops on the stack
        else {
            theop = (*parsedinfo)[eindex]->GetType();
            if (theop=='~') tstack->Not();
            else if (theop=='&') tstack->And();
            else if (theop=='|') tstack->Or();
            else if (theop=='%') tstack->Xor();
            else assert(false);
        }
    }
    // done, we now get the result and clear
    assert(tstack->One());
    fresult = tstack->top();
    delete tstack;
    // done
    return fresult;
}
//**************************************************************************
