/******************************************************************************
Name    :   ao3_specials3.cpp
Author  :   John Q Metro
Purpose :   AO3 fandom and kudos filters
Created :   May 14, 2014
Updated :   August 26, 2023
******************************************************************************/
#ifndef AO3_SPECIALS3_H
  #include "ao3_specials3.h"
#endif // AO3_SPECIALS3_H
//------------------------------
#ifndef UTILS2_H_INCLUDED
  #include "../../utils/utils2.h"
#endif // UTILS2_H_INCLUDED
#ifndef AO3_FICOBJ_H_INCLUDED
  #include "../../../fanfic/data/ao3/ao3_ficobj.h"
#endif // AO3_FICOBJ_H_INCLUDED

#ifndef JFFNFICOBJ
  #include "../../../fanficnet/data/ffn_ficobj.h"
#endif // BASEOBJ_H_INCLUDED


#ifndef JFEXPRMATCH
  #include "../../express/expr_match.h"
#endif
#ifndef JFEXPRPARSE
  #include "../../express/expr_parse.h"
#endif
#include <assert.h>
/*****************************************************************************/
const jfFilterTypeMeta AO3_FANDOM_FILTER_INFO =
    jfFilterTypeMeta(jfFilterTypeGroup::FANDOM, "AO3FandomFilter", "AO3 Fandom Filter",
          QString("A Filter that uses the AO3 Fandom Names with the fic. The General") +
                  " Expression filter works on the list of fandoms as a whole, while " +
                  "the other filter passes fics that either one fandom or all fandoms" +
                  " match the expression. This can be used, for example, to make sure" +
                  " crossovers are not just within a specific 'verse, like Star Trek.",
          IdForAO3Fanfic(), createFilter<jfAO3FandomFilter> );

// =========================================================================
/* Intended replacement for jfACategoriesFilter, with crossover boolean, forall
filter (mainly intended to allow excluding intra-universe crossovers) and the
usual expression filter operating on the names of all fandoms */
//+++++++++++++++++++++++++++++++++++++++
// constructors
jfAO3FandomFilter::jfAO3FandomFilter(const QString& filter_name):jfFilterBase(filter_name) {
    loaded = false;
    allmatch = false;
    must_crossover = false;
    // setting parsed stuff
    perfmatch_parsed = NULL;
    mainmatch_parsed = NULL;
}
//---------------------------------
jfAO3FandomFilter::jfAO3FandomFilter(QString&& filter_name):jfFilterBase(filter_name) {
    loaded = false;
    allmatch = false;
    must_crossover = false;
    // setting parsed stuff
    perfmatch_parsed = NULL;
    mainmatch_parsed = NULL;
}
//--------------------------------
jfAO3FandomFilter::jfAO3FandomFilter(const jfAO3FandomFilter& source):jfFilterBase(source.name) {
    // copying the most basic items
    description = source.description;
    // copying the booleans
    must_crossover = source.must_crossover;
    // copying per fandom stuff
    allmatch = source.allmatch;
    perfmatch_src = source.perfmatch_src;
    if (source.perfmatch_parsed!=NULL) {
        perfmatch_parsed = (source.perfmatch_parsed)->Copy();
    }
    else perfmatch_parsed = NULL;
    // copying general match stuff
    mainmatch_src = source.mainmatch_src;
    if (source.mainmatch_parsed!=NULL) {
        mainmatch_parsed = (source.mainmatch_parsed)->Copy();
    }
    else mainmatch_parsed = NULL;
}
//+++++++++++++++++++++++++++++++++++++++
//checks if the filter has no effect
bool jfAO3FandomFilter::IsEmpty() const {
  if (must_crossover) return false;
  bool pempty = (perfmatch_parsed == NULL) || (perfmatch_parsed->size() == 0);
  bool mempty = (mainmatch_parsed == NULL) || (mainmatch_parsed->size() == 0);

  // note that the perfmatch stuff only has meaning if must_crossover is true
  return pempty && mempty;
}
//+++++++++++++++++++++++++++++++++++++++
QString jfAO3FandomFilter::ToString() const {
    QString result = Bool2Str(must_crossover) + ";";
    result += Bool2Str(allmatch) + ";";
    result += EscapeNL(perfmatch_src,EXPR_DELIMS) + ";";
    result += EscapeNL(mainmatch_src,EXPR_DELIMS);
    return result;
}
//+++++++++++++++++++++++++++++++++++++++
// specific data setting methods
//--------------------------------
// this 'empties out' the parameter
bool jfAO3FandomFilter::GeneralFromExpr(jfSimpleExpr* in_source) {
    if ((in_source != NULL) && (in_source->valid)) {
        if (mainmatch_parsed != NULL) delete mainmatch_parsed;
        if (in_source->isempty) {
            mainmatch_parsed = NULL;
            mainmatch_src = "";
        }
        else {
            mainmatch_parsed = in_source->parsedinfo;
            mainmatch_src = in_source->srcstring;
        }

        in_source->parsedinfo = NULL;
        in_source->valid = false;
        return true;
    }
    else return false;
}
//--------------------------------
bool jfAO3FandomFilter::PerFandomFromExpr(jfSimpleExpr* in_source) {

    if ((in_source == NULL) || (in_source->isEmpty())) {
        if (perfmatch_parsed!=NULL) delete perfmatch_parsed;
        perfmatch_parsed = NULL;
        perfmatch_src = "";
        return true;
    }
    else if ((!in_source->IsValid())) return false;
    else {
        if (perfmatch_parsed!=NULL) delete perfmatch_parsed;
        perfmatch_parsed = in_source->parsedinfo;
        in_source->parsedinfo = NULL;
        in_source->valid = false;
        perfmatch_src = in_source->srcstring;
        return true;
    }
}
//--------------------------------
void jfAO3FandomFilter::SetCrossover(bool iscross) {
  must_crossover = iscross;
}
//--------------------------------
void jfAO3FandomFilter::SetAllMatch(bool amatch) {
  allmatch = amatch;
}
//+++++++++++++++++++++++++++++++++++++++
// getting data
//--------------------------------
QString jfAO3FandomFilter::GetMainSrc() const {
  return mainmatch_src;
}
//--------------------------------
QString jfAO3FandomFilter::GetPerFandomSrc() const {
  return perfmatch_src;
}
//--------------------------------
bool jfAO3FandomFilter::GetCrossover() const {
  return must_crossover;
}
//--------------------------------
bool jfAO3FandomFilter::GetAllMatch() const {
  return allmatch;
}
//+++++++++++++++++++++++++++++++++++++++
const jfFilterTypeMeta& jfAO3FandomFilter::GetTypeMetaInfo() const {
    return AO3_FANDOM_FILTER_INFO;
}
// -------------------------------
jfFilterBase* jfAO3FandomFilter::GenCopy() const {
  jfAO3FandomFilter* result;
  result = new jfAO3FandomFilter(*this);
  return result;
}
//+++++++++++++++++++++++++++++++++++++++
// destructor
jfAO3FandomFilter::~jfAO3FandomFilter() {
  if (mainmatch_parsed!=NULL) delete mainmatch_parsed;
  if (perfmatch_parsed!=NULL) delete perfmatch_parsed;
}
//+++++++++++++++++++++++++++++++++++++++
bool jfAO3FandomFilter::FromStringInner(const QString& sourcedata, QString& error_out) {
    // constants
    const QString fname = "jfAO3FandomFilter::FromStringInner";
    // starting with the parsing
    jfLineParse lparse(sourcedata);

    // for the new format, we require 4 fields
    if (lparse.NNotX(4)) {
        error_out = "The number of fields is not 4!";
        return false;
    }
    // the flags
    bool new_must_crossover, new_allmatch;
    if ((!lparse.BoolVal(0,new_must_crossover)) || (!lparse.BoolVal(1, new_allmatch))) {
        error_out = "Invalid flags";
        return false;
    }

    // per filter match
    jfElemArray* per_filter_exp = NULL;
    QString oerr;
    QString new_perf_src = lparse.UnEscStrE(2);
    if (!new_perf_src.isEmpty()) {
        per_filter_exp = ParseInput(new_perf_src,oerr);
        if (per_filter_exp == NULL) {
            error_out = oerr;
            return false;
        }
    }

    // merged match
    jfElemArray* merged_exp = NULL;
    QString new_merged_src = lparse.UnEscStrE(3);
    if (!new_merged_src.isEmpty()) {
        merged_exp = ParseInput(new_merged_src,oerr);
        if (merged_exp == NULL) {
            error_out = oerr;
            if (per_filter_exp != NULL) delete per_filter_exp;
            return false;
        }
    }

    // if we get here, everything is okay. we assign values
    mainmatch_src = new_merged_src;
    if (mainmatch_parsed!=NULL) delete mainmatch_parsed;
    mainmatch_parsed = merged_exp;

    perfmatch_src = new_perf_src;
    if (perfmatch_parsed!=NULL) delete perfmatch_parsed;
    perfmatch_parsed = per_filter_exp;

    must_crossover = new_must_crossover;
    allmatch = new_allmatch;

    // done
    loaded = true;
    return true;
}
// ----------------------------------------
// the core matching method
bool jfAO3FandomFilter::CoreMatch(const jfSearchResultItem* testelem) const {
    // variables
    const jfAO3Fanfic* typed_item;
    QString currfandom;
    bool expr_res;
    size_t findex,fcount;

    // starting
    typed_item = dynamic_cast<const jfAO3Fanfic*>(testelem);
    // the simplest test is the crossover test
    if (must_crossover) {
        if ((typed_item->FandomCount())==1) return false;
    }
    // nest up, the mainmatch
    if (mainmatch_parsed!=NULL) {
        currfandom = typed_item->GetFandoms();
        expr_res = StringExprMatch(currfandom,mainmatch_parsed);
        if (!expr_res) return false;
    }
    // testing the crossovers!
    if (perfmatch_parsed!=NULL) {
        // prepping for the loop
        fcount = typed_item->FandomCount();
        assert(fcount != 0);
        // the matching loop
        for (findex=0;findex<fcount;findex++) {
            // checking for the match
            currfandom = typed_item->GetFandom(findex);
            expr_res = StringExprMatch(currfandom,perfmatch_parsed);

            // we now use allmatch. if allmatch is true, then one faliure spells doom
            if (allmatch) {
                if (!expr_res) return false;
            }
            // otherwise, one success means success
            else {
                if (expr_res) return true;
            }
        }
        // if we get here..
        return allmatch;
    }
    // everything has passed
    return true;
}
//+++++++++++++++++++++++++++++++++++++++
// file i/o
//--------------------------------
// the old format was multiline...
bool jfAO3FandomFilter::CompatReadRestFromFile(jfFileReader* infile) {
    // basic stuff
    const QString fname = "jfAO3FandomFilter::CompatReadRestFromFile";
    assert(infile!=NULL);
    QString oerr,cline;

    // starting the read, boolean values
    if (!infile->ReadParseLine(2,fname)) return false;
    if (!(infile->lp).BoolVal(0,must_crossover)) {
      return infile->BuildError("The crossover field is not a boolean!");
    }
    if (!(infile->lp).BoolVal(1,allmatch)) {
      return infile->BuildError("The all fandoms match field is not a boolean!");
    }
    // next, the per fandom expression
    if (!infile->ReadUnEsc(cline,fname)) return false;
    perfmatch_src = cline;
    if (perfmatch_src.isEmpty()) perfmatch_parsed = NULL;
    else {
      perfmatch_parsed = ParseInput(cline,oerr);
      if (perfmatch_parsed==NULL) {
        return infile->BuildError("PF Parsing Error : "+oerr);
      }
    }
    // next, the main match expression
    if (!infile->ReadUnEsc(cline,fname)) return false;
    mainmatch_src = cline;
    if (mainmatch_src.isEmpty()) mainmatch_parsed = NULL;
    else {
      mainmatch_parsed = ParseInput(cline,oerr);
      if (mainmatch_parsed==NULL) {
        return infile->BuildError("MM Parsing Error : "+oerr);
      }
    }
    // done
    return true;
}

//+++++++++++++++++++++++++++++++++++++++
// extra methods
jfElemArray* jfAO3FandomFilter::ParseInput(QString srcstring, QString& oerr) const {
  // constants
  const QString fname = "jfAO3FandomFilter::ParseInput";
  // variables
  jfExpParserClass* theparser;
  jfElemArray* pfix, *xfix;
  size_t osize;

  // starting
  srcstring = srcstring.trimmed();
  theparser = new jfExpParserClass(srcstring,true,NULL);
  // trying the parse
  if (!theparser->ParseExpression(osize)) {
    // the intial parse has failed
    oerr = theparser->parse_error;
    delete theparser;
    return NULL;
  }

  // parsed okay
  xfix = theparser->GetResult();
  assert(xfix!=NULL);
  delete theparser;
  // next, we postfixize it
  pfix = MakeExprPostfix(xfix,oerr);
  delete xfix;
  return pfix;
}

//============================================================
const jfFilterTypeMeta FAVS_FILTER_INFO =
    jfFilterTypeMeta(jfFilterTypeGroup::FAVS, "KudoCountFilter", "Favs/Kudos Filter",
          QString("For items that have favs or kudos, this filter passes if ") +
                  "the kudo/favs count is between min and max, inclusive, or if " +
                  "max is zero, the kudo/favs count is greater than min.",
          { IdForFFNItemCore(), IdForAO3Fanfic()},
          createFilter<jfFavKudoFilter> );

// constructors
//---------------------------------------------------
jfFavKudoFilter::jfFavKudoFilter(const QString& name):jfMinMaxUFilter(name) {
}
//---------------------------------------------------
jfFavKudoFilter::jfFavKudoFilter(const QString& name, size_t inmin, size_t inmax)
    :jfMinMaxUFilter(name, inmin,inmax) {
}
//---------------------------------------------------
jfFavKudoFilter::jfFavKudoFilter(const jfFavKudoFilter& source):jfMinMaxUFilter(source.name) {
    // copying the most basic items
    name = source.name;
    description = source.description;
    min = source.min;
    max = source.max;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// returns the list of element names campatible with this filter
//---------------------------------------------------
const jfFilterTypeMeta& jfFavKudoFilter::GetTypeMetaInfo() const {
    return FAVS_FILTER_INFO;
}
// -----------------------------------------------------------------
/* Overriden because this filter replaces two older filters but is otherwise the same,
 * so matching the old ids will allow compatability when loading old saved searches. */
bool jfFavKudoFilter::MatchTypeIdentifier(const QString to_match) const {
    if (GetTypeMetaInfo().Identifier() == to_match) return true;
    else if ("AO3KudoCountFilter" == to_match) return true;
    else if ("FFNFavsFilter" == to_match) return true;
    else return false;
}
//---------------------------------------------------
jfFilterBase* jfFavKudoFilter::GenCopy() const {
  jfFavKudoFilter* result = new jfFavKudoFilter(name,min,max);
  result->description = description;
  return result;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the core matching method
bool jfFavKudoFilter::CoreMatch(const jfSearchResultItem* testelem) const {
  // variables
  const jfAO3Fanfic* rvalue;
  const jfFFNItemCore *fvalue;
  size_t cvalue;
  // checks
  assert(testelem!=NULL);
  // determining the type
  if (testelem->GetTypeLabels().contains(IdForAO3Fanfic())) {
      rvalue = dynamic_cast<const jfAO3Fanfic*>(testelem);
      cvalue = rvalue->GetKudos();
  }
  else if (testelem->GetTypeLabels().contains(IdForFFNItemCore())) {
      fvalue = dynamic_cast<const jfFFNItemCore*>(testelem);
      cvalue = fvalue->GetFavs();
  }
  else assert(false);

  // checking the wordcount
  return TestMatch(cvalue);
}
/*****************************************************************************/
