/////////////////////////////////////////////////////////////////////////////
// Name:        expr_parse.h
// Author :     John Q Metro
// Purpose :    Declares functions and stuff for parsing boolean expressions
// Created:     31.12.06
// Conversion to QT Started April 8, 2013
// Updated:     August 6, 2022 (adding whole string matching)
/////////////////////////////////////////////////////////////////////////////
//headers
//----------------------------------------------------------------------------
#ifndef JFEXPRSTRUCTS
#include "expr_structs.h"
#endif
#define JFEXPRPARSE
//*****************************************************************************
/* The source format of the tokens is as follows

 FILTER LITERAL TOKENS: start and end with '#'
 - The first part is the filter type, the second a
 string representation of the data contents.

 FILTER REFERENCE TOKENS: start and end with '@'
 - for global reference filter tokens, the first part is the map name, the second the
 filter name. The two parts are separated by ';'
 - for local filter tokens, the contents are just the local filter name.

 STRING TOKENS: by default, they are substring case insensetive
 -However, a sort of 'id' at the start, followed by ';' can change this.
 -This id cannot be more than 2 characters long.
 -character it can contain: 'c' or 'i' to set case sensetivity
 -Also, 's', 'm', 't', 'q', or 'z' to set the type
 */
//*****************************************************************************
// match type
enum jfExpMatchType:char { jfemt_SUBSTRING = 's', jfemt_TOKENIZED = 'm', jfemt_WHOLESTRING = 'z' };

// because of the complication of parsing, using a class can make parts simpler
class jfExpParserClass {
  public:
    QString parse_error;
    // the constructor
    jfExpParserClass(const QString& insource,bool issimple, const jfFilterMap* inlocalmap);
    // activation
    bool ParseExpression(size_t& outsize);
    // getting the result
    jfElemArray* GetResult();
    QString GetRawExp() const;
    // done
    ~jfExpParserClass();
  protected:
    // constants
    QString fname1;
    // used in parsing
    QString rawexp;
    bool started, done, gotten;
		bool simple;
    jfExprToker* tokenizer;
    jfElemArray* theresult;
    const jfFilterMap* localmap;
    // private parsing helper
    bool MakeError(const QString& msg, bool back=false);
    // internal parsing methods
    jfOperandElem* MakeOperand(const QString& source);
    bool IsLiteralFilter(QString& sourcef);
    bool IsFilterReference(QString& sourcef);
    bool FilterTest(QString& sourcef, QChar delimt) const;
    bool CheckId(const QString& idin, jfExpMatchType& ssout, bool& csout, bool& titout) const;
};
//=============================================================================
// following function is designed to take in an element array on produce a postfix
// expression verion
jfElemArray* MakeExprPostfix(jfElemArray* infixsrc, QString& outerr);
//****************************************************************************
