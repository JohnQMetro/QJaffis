//***************************************************************************
// Name:        list_sexp.cpp
// Author :     John Q Metro
// Purpose :    String expression based filters for lists of strings
// Created:     February 26, 2023
// Updated:     February 26, 2023
//***************************************************************************
#include "list_sexp.h"

#ifndef FICBASE_H
    #include "../../../fanfic/data/ficbase.h"
#endif // FICBASE_H
//***************************************************************************
// the constructors
//----------------------------------------------------------------------
// the basic constructor
jfListExprFilterBase::jfListExprFilterBase():jfBaseFilter() {
    parsed_expression = NULL;
    match_mode = jfListMatchMode::MATCH_MERGED;
    validdata = true;
}
//----------------------------------------------------------------------
jfListExprFilterBase::jfListExprFilterBase(const QString& sourcedata):jfBaseFilter() {
    parsed_expression = NULL;
    match_mode = jfListMatchMode::MATCH_MERGED;
    validdata = false;
    FromString(sourcedata);
}
//----------------------------------------------------------------------
jfListExprFilterBase::jfListExprFilterBase(jfListMatchMode in_match_mode, jfSimpleExpr* in_source):jfBaseFilter() {
    parsed_expression = NULL;
    match_mode = in_match_mode;
    validdata = false;
    FromExpr(in_source);
}
//----------------------------------------------------------------------
jfListExprFilterBase::jfListExprFilterBase(const jfListExprFilterBase& source):jfBaseFilter() {
    num_id = source.num_id;
    validdata = source.validdata;
    name = source.name;
    description = source.description;
    match_mode = source.match_mode;
    if (source.parsed_expression == NULL) {
        parsed_expression = NULL;
    }
    else {
        parsed_expression = source.parsed_expression->Copy();
    }
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfListExprFilterBase::isEmpty() const {
    return (parsed_expression == NULL) || (parsed_expression->isEmpty());
}
//-----------------------------------------------------------------------------
// returns a general filter type. The default is 0, unless overriden
size_t jfListExprFilterBase::GetFilType() const { return 1;}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// redefined virtual functions
// loading stuff
bool jfListExprFilterBase::FromString(const QString& sourcedata) {
    // constants
    const QString fname = "jfListExprFilterBase::FromString";

    if (sourcedata.isEmpty()) {
        EmptyFilter();
        match_mode = jfListMatchMode::MATCH_MERGED;
        validdata = true;
        return true;
    }
    else {
       jfLineParse* lparser = new jfLineParse(sourcedata);
       if (lparser->NNotX(2)) {
           delete lparser;
           return false;
       }
       // the first field should be the match mode
       jfListMatchMode in_match_mode = StringToMode(lparser->Get(0));
       jfSimpleExpr* new_expr = new jfSimpleExpr(lparser->UnEscStr(1));
       delete lparser;
       bool okay_expr = (new_expr->IsValid()) || (new_expr->isEmpty());
       if (okay_expr) {
           EmptyFilter();
           match_mode = in_match_mode;
           validdata = true;
           if (new_expr->isEmpty()) parsed_expression = NULL;
           else parsed_expression = new_expr;
       }
       else delete new_expr;
       return okay_expr;
    }
}
//----------------------------------------------------------------------
QString jfListExprFilterBase::ToString() const {
    if (parsed_expression == NULL) return "";
    else {
        jfOutString resb;
        resb << ModeToString(match_mode) << parsed_expression->GetSrc();
        return resb;
    }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// modifying and checking the contents
//----------------------------------------------------------------------
jfListMatchMode jfListExprFilterBase::GetMatchMode() const {
    return match_mode;
}
//----------------------------------------------------------------------
void jfListExprFilterBase::SetMatchMode(jfListMatchMode in_match_mode) {
    match_mode = in_match_mode;
}
//----------------------------------------------------------------------
bool jfListExprFilterBase::FromExpr(jfSimpleExpr* in_source) {
  if (in_source==NULL) return false;
  else {
      parsed_expression = in_source->Copy();
      validdata = parsed_expression->IsValid();
  }
  return true;
}
//----------------------------------------------------------------------
// returning a string representation of the parsed contents
QString jfListExprFilterBase::MakePList() const {
    if (parsed_expression == NULL) return "";
    else return parsed_expression->MakePList();
}
//-----------------------------------------------------------------------
// sort of like load string, except the string is empty
void jfListExprFilterBase::EmptyFilter() {
    if (parsed_expression != NULL) {
        delete parsed_expression;
        parsed_expression = NULL;
    }
    validdata = true;
}
//-----------------------------------------------------------------------
QString jfListExprFilterBase::GetExpression() const {
    if (parsed_expression == NULL) return "";
    else return parsed_expression->GetSrc();
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the destructor
jfListExprFilterBase::~jfListExprFilterBase() {
    EmptyFilter();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QString jfListExprFilterBase::ModeToString(jfListMatchMode in_match_mode) const {
    if (in_match_mode == jfListMatchMode::MATCH_ALL) return "ALL";
    else if (in_match_mode == jfListMatchMode::MATCH_ONE) return "ONE";
    else if (in_match_mode == jfListMatchMode::MATCH_75P) return "75P";
    else return "MERGED";
}
//----------------------------------------------------------------------
jfListMatchMode jfListExprFilterBase::StringToMode(const QString& mode_source) const {
    if (mode_source.compare("ALL", Qt::CaseInsensitive) == 0) return jfListMatchMode::MATCH_ALL;
    else if (mode_source.compare("ONE", Qt::CaseInsensitive) == 0) return jfListMatchMode::MATCH_ONE;
    else if (mode_source.compare("75P", Qt::CaseInsensitive) == 0) return jfListMatchMode::MATCH_75P;
    else return jfListMatchMode::MATCH_MERGED;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// match methods
//----------------------------------------------------------------------
bool jfListExprFilterBase::MergeMatch(const QStringList& to_match) const {
    QString joined_list = to_match.join(QChar(','));
    return parsed_expression->InternalMatch(joined_list);
}
//----------------------------------------------------------------------
bool jfListExprFilterBase::ItemwiseMatchAll(const QStringList& to_match) const {
    for (QString item : to_match) {
        bool did_match = parsed_expression->InternalMatch(item);
        if (! did_match) return false;
    }
    return true;
}
//----------------------------------------------------------------------
bool jfListExprFilterBase::ItemwiseMatchAny(const QStringList& to_match) const {
    for (QString item : to_match) {
        bool did_match = parsed_expression->InternalMatch(item);
        if (did_match) return true;
    }
    return false;
}
//----------------------------------------------------------------------
int jfListExprFilterBase::ItemwiseMatchCount(const QStringList& to_match) const {
    int match_count = 0;
    for (QString item : to_match) {
        bool did_match = parsed_expression->InternalMatch(item);
        if (did_match) match_count++;
    }
    return match_count;
}
//----------------------------------------------------------------------
bool jfListExprFilterBase::InternalMatch(const QStringList& to_match) const {
    if ((parsed_expression == NULL) || (parsed_expression->isEmpty())) return true;
    else if (to_match.isEmpty()) return false;
    else {
        if (match_mode == jfListMatchMode::MATCH_ALL) {
            return ItemwiseMatchAll(to_match);
        }
        else if (match_mode == jfListMatchMode::MATCH_ONE) {
            return ItemwiseMatchAny(to_match);
        }
        else if (match_mode == jfListMatchMode::MATCH_75P) {
            int mcount = ItemwiseMatchCount(to_match);
            if (mcount == to_match.size()) return true;
            else {
                double frac = ((double)mcount) / (double)(to_match.size());
                return frac >= 0.75;
            }
        }
        else {
            return MergeMatch(to_match);
        }
    }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
size_t jfListExprFilterBase::ExtraLines() const {
  return 1;
}
//-----------------------------------------------------------------------
bool jfListExprFilterBase::AddRestToFile(QTextStream* outfile) const {
  QString buffer;
  // checking and special conditions
  if (outfile==NULL) return false;
  // composing line 4
  (*outfile) << ToString() << "\n";
  return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfListExprFilterBase::ReadRestFromFile(jfFileReader* infile) {
  const QString funcname = "jfListExprFilterBase::ReadRestFromFile";
  // input data
  QString cline;
  bool resx;
  // starting checks (and reading the line)
  assert(infile!=NULL);
  if (!infile->ReadLine(cline, funcname)) return false;
  resx = FromString(cline);
  if (!resx) return infile->BuildError("The source or expression string is invalid!");
  else return true;
}
//-----------------------------------------------------------------------
void jfListExprFilterBase::CoreCopy(const jfListExprFilterBase& source) {
    num_id = source.num_id;
    validdata = source.validdata;
    name = source.name;
    description = source.description;
    match_mode = source.match_mode;
    if (source.parsed_expression == NULL) {
        parsed_expression = NULL;
    }
    else {
        parsed_expression = source.parsed_expression->Copy();
    }
}
// ==========================================================================================
// constructors
//----------------------------------------------------------
jfCharListExprFilter::jfCharListExprFilter():jfListExprFilterBase() {}
//----------------------------------------------------------
jfCharListExprFilter::jfCharListExprFilter(const jfCharListExprFilter& source):jfListExprFilterBase(source) {

}
//----------------------------------------------------------
jfCharListExprFilter::jfCharListExprFilter(jfListMatchMode in_match_mode, jfSimpleExpr* in_source):jfListExprFilterBase(in_match_mode, in_source) {}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QString jfCharListExprFilter::GetTypeDescription() const {
  return "The Character List Expression Filter matches the list of characters against a boolean \
expression, the elements of which are themselves to be matched. These elements \
are either strings or substrings. The user can choose to match a merged list, all characters, or at least one character.";
}
//------------------------------------------------------------
jfCharListExprFilter* jfCharListExprFilter::Copy() const {
  return new jfCharListExprFilter(*this);
}
//------------------------------------------------------------
jfBaseFilter* jfCharListExprFilter::GenCopy() const {
  return Copy();
}
//------------------------------------------------------------
QString jfCharListExprFilter::GetTypeID() const {
  return "CharacterListFilter";
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the core matching method
bool jfCharListExprFilter::CoreMatch(const jfBasePD* testelem) const {
  const jfGenericFanfic2* fanfic_item = NULL;
  // checks and starts
  assert(testelem!=NULL);
  // two types
  QString datatype = testelem->GetTypeID();
  if ((datatype == "FFNItem") || (datatype=="AO3Fanfic") || (datatype=="FIM_Fanfic")) {
      fanfic_item = dynamic_cast<const jfGenericFanfic2*>(testelem);
      return InternalMatch(fanfic_item->GetCharacterList());
  }
  else assert(false);
}

//*******************************************************************************************
