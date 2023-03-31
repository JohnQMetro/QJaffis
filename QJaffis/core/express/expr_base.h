/////////////////////////////////////////////////////////////////////////////
// Name:        expr_base.h
// Author :     John Q Metro
// Purpose :    Declares basic classes for expression elements
// Created:     26.12.06
// Conversion to QT Started April 8, 2013
// Updated:     August 6, 2022 (added jfWholeStringElem)
/////////////////////////////////////////////////////////////////////////////
// headers
#ifndef JFBASEFILTER
#include "../filters/base/filterbase.h"
#endif

#ifndef BASECOLLECT_H_INCLUDED
    #include "../structs/basecollect.h"
#endif // BASECOLLECT_H_INCLUDED


#define JFEXPRBASE
//*****************************************************************************
extern const QString expr_operators;
extern const QString expr_operands;
extern const QString expr_precstring;
extern const QString expr_scitems;
//=============================================================================
// a forward declaration
class jfExprElem;
//-----------------------------------------------------------------------------
// a special function for copying an element
jfExprElem* CopyElement(jfExprElem* source);
//----------------------------------------------------------------------------
// getting precedence
size_t GetPrec(const QChar& intype);
//============================================================================
/* for name verification, the filter links can be treated as a tree. VerifyNames
 is used to probe this tree for problems. Only one branch is stored at a time,
 with backup up occurring when we reach the end of a branch.

 Since there are two ways to refer to a filter, there are two lists, local and
 global. No global expression is supposed to refer to a local filter, an object
 of this class stores the info required to detect that.

 Another error is not finding a filter, this is done in VerifyNames but we do
 not need a jfNameVerif object to detect this.

 For cicular references, we use a jfNameVerif object to hold a history list, and
 the VerfifyNames method searches for the current filter in the list. If it
 is there, this is an error. otherwise, we add the current filter to this list
 and call the Expression Filter's NameVerif method to check any embedded filter
 references.

 Note that if the current filter is *not* an Expression Filter, it is the end of
 a branch and all we need to do is check for local vs global and if the filter
 is there.
 */
// a special class used in verification
class jfNameVerif {
public:
	jfNameVerif();
	// some special error handling methods
	bool MakeChainString();
  QString GetChainString() const;
	bool InGlobal() const;
	// looking for names
  bool FindName(const QString& fname);
  bool FindName(const QString& mapname, const QString& fname);
	// name push and pop
  void PushGlobal(const QString& mapname, const QString& fname);
  bool PushLocal(const QString& fname);
	bool PopName();
	// setting and getting the error
	bool InError() const;
  void SetLocalError(const QString& lfiltname);
  void SetNotFoundError(const QString& fname);
  void SetNotFoundError(const QString& mapname, const QString& fname);
  void SetCircularError(const QString& fname);
  void SetCircularError(const QString& mapname, const QString& fname);
  QString GetError() const;
	// the destructor
	~jfNameVerif();
protected:
    // local list
    QStringList* local_list;
    size_t local_count;
    // global list
    QStringList* global_list;
    size_t global_count;
    // error stuff
    QString chainstring;
    QString errormsg;
    bool inerror;
};
//=============================================================================
// the base type
class jfExprElem {
public:
    QChar GetType() const;
    virtual bool IsOperand() const = 0;
    virtual QString String() const = 0;
    virtual ~jfExprElem();
protected:
    // internal base data
    QChar type;
};
//=============================================================================
// USABLE! --- For holding operators and also ()
class jfOperatorElem : public jfExprElem {
public:
    jfOperatorElem(QChar typestr);
    jfOperatorElem(const jfOperatorElem& source);
    virtual bool IsOperand() const;
    virtual QString String() const;
};
//============================================================================
// a default operand class
class jfOperandElem : public jfExprElem {
public:
    virtual bool IsOperand() const;
    virtual bool IsString() const = 0;
    bool IsValid() const;
protected:
    bool valid;
};
//============================================================================
// basic operand class for string matching
class jfStrElem : public jfOperandElem {
public:
    // info methods
    virtual bool IsString() const;
    QString GetPattern() const;
    bool GetCasesen() const;
    bool GetTitle() const;
    virtual QString String() const;
protected:
    QString pattern;
    bool casesen;
    bool title;
};
//==========================================================================
// USABLE! --- Substring matching operand
class jfSubstringElem : public jfStrElem {
public:
    jfSubstringElem(const QString& newpattern, bool newcs=false, bool istitle=false);
    jfSubstringElem(const jfSubstringElem& source);
    bool Match(const QString& base) const;
};
//==========================================================================
// USABLE! --- Token string matching operand
class jfTokenMElem : public jfStrElem {
public:
    jfTokenMElem(const QString& newpattern, bool newcs=false, bool istitle=false);
    jfTokenMElem(const jfTokenMElem& source);
    bool Match(const QStringList* base) const;
    ~jfTokenMElem();
protected:
    QStringList* tdata;
};
//===========================================================================
// USABLE, --- Match entire input string, meant for use with individual tags
class jfWholeStringElem : public jfStrElem {
public:
    jfWholeStringElem(const QString& newpattern, bool newcs=false);
    jfWholeStringElem(const jfWholeStringElem& source);
    bool Match(const QString& base) const;
};
//=============================================================================
/* Operand class for embedded filter literals. We disallow embedded expression
 filters (they are redundant). This simplifies things quite a bit. */
class jfLiteralFilterElem : public jfOperandElem {
public:
    // constructors
    jfLiteralFilterElem(const QString& ftype, const QString& rawdata);
    jfLiteralFilterElem(const jfLiteralFilterElem& fsource);
    // public methods
    bool Match(const jfSearchResultItem* testelem) const;
    // virtual methods that need to be defined
    virtual QString String() const;
    virtual bool IsString() const;
    // destuctor
    ~jfLiteralFilterElem();
protected:
    QString filttype, source;
    jfBaseFilter* thefilter;
};
//=============================================================================
/* the basic class for filter references, either to a local or global source.
 The filter is stored externally. Since the referenced filter can be an expression
 with filter references of it's own, we can get a problems with reference filters
 being renamed or deleted, or with circular references. VerifyNames is used to check
 that. Also, CopyLoad is used to copy the filter before actual searching starts */
class jfFilterElem : public jfOperandElem {
public:
    virtual bool IsString() const;
    bool Match(const jfSearchResultItem* testelem) const;
    virtual bool IsExpr() const = 0;
    virtual bool VerfyNames(jfNameVerif* nameobj) = 0;
    virtual bool CopyLoad() = 0;
protected:
    jfBaseFilter* thefilter;
    QString filter_name;
};
//=============================================================================
// a forward declaration
class jfFilterPointer;
//-----------------------------------------------------------------------------
// USABLE! --- Global Filter Reference
class jfGlobalFilterElem : public jfFilterElem {
public:
    jfGlobalFilterElem(const QString& inmapname, const QString& filtername);
    jfGlobalFilterElem(const jfGlobalFilterElem& source);
    virtual bool IsExpr() const;
    virtual bool VerfyNames(jfNameVerif* nameobj);
    virtual bool CopyLoad();
    virtual QString String() const;
    ~jfGlobalFilterElem();
protected:
    QString mapname;
    jfFilterPointer* c_pointer;
};
//=============================================================================
// USABLE ! ---- Local Filter Reference
class jfLocalFilterElem : public jfFilterElem {
public:
    jfLocalFilterElem(jfFilterMap* inlocalmap, const QString& filtername);
    jfLocalFilterElem(const jfLocalFilterElem& source);
    virtual bool IsExpr() const;
    virtual bool VerfyNames(jfNameVerif* nameobj);
    virtual bool CopyLoad();
    virtual QString String() const;
    ~jfLocalFilterElem();
protected:
    jfFilterMap* localmap;
};

//****************************************************************************
