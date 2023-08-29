//////////////////////////////////////////////////////////////////
// Name:        expr_structs.h
// Author :     John Q Metro
// Purpose :    Declares basic structures for expressions
// Created:     30.12.06
// Conversion to QT Started April 8, 2013
// Updated:     March 29, 2009
/////////////////////////////////////////////////////////////////////////////

// for all others, include the necessary headers
// ----------------------------------------------------------------------------
#include <stack>
#include <vector>
//----------------------------------------------------------------------------
#ifndef JFEXPRBASE
#include "expr_base.h"
#endif
#define JFEXPRSTRUCTS
//*****************************************************************************
// a class that holds a stack of operators, for use in postfixing
class jfOperatorStack : public std::stack<QChar> {
public:
    bool IsLParen() const;
    bool NotLParen() const;
    bool IsOp() const;
    void PushElem(const jfExprElem* operat);
    jfExprElem* PopElem();
    QChar Get();
};
//===========================================================================
// the class that holds elements
class jfElemArray : public std::vector<jfExprElem*> {
public:
    jfElemArray();
    jfElemArray(const jfElemArray& source);
    // Adding Items
    size_t Append(jfExprElem* newitem);
    size_t Append(const QChar& newitem);
    // basic info
    size_t GetOpCount() const;
    size_t GetTokCount() const;
    size_t GetTTokCount() const;
    bool NotOperand(const size_t& index) const;
    // Specials
    bool VerifyNames(jfNameVerif* nameobj) const;
    bool CopyLoad();
    // clearing stuff
    size_t DeleteAll();
    // copying
    jfElemArray* Copy() const;
protected:
    size_t opcount,tokcount,ttokcount;
};
//============================================================================
// a class that tokenized basic expressions in string form into tokens
class jfExprToker {
public:
    jfExprToker(const QString& sourceexpr);
    // getting stuff
    QString GetToken();
    QChar GetTChar();
    // basic info
    bool TokensLeft() const;
    bool AtOp() const;
    size_t GetIndex() const;
    size_t LastIndex() const;
protected:
    QString source;
    size_t index, lindex, sleng;
    bool pastend;
};
//============================================================================
// a special boolean valued stack that has built in logical operators
class jfTruthStack : public std::stack<bool> {
public:
    // logical operators
    void And();
    void Or();
    void Xor();
    void Not();
    // special methods
    bool One() const;
};

//****************************************************************************
