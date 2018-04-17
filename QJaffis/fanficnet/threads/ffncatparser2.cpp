/*****************************************************************************
Name    : ffncatparser2.cpp
Basic   : Fanfiction.Net category parsing, crossover versions
Author  : John Q Metro
Started : July 20, 2016
Updated : July 20, 2016

******************************************************************************/
#ifndef FFNCATPARSER2_H
  #include "ffncatparser2.h"
#endif // FFNCATPARSER2_H
//-----------------------------------
#include <assert.h>

/******************************************************************************/
// --- { METHODS for jfFFNCrossoverSectionParser] ------------------------------
//++++++++++++++++++++++++++++++++++++++++++++
// constructor
jfFFNCrossoverSectionParser::jfFFNCrossoverSectionParser():jfFFNCatParserBase() {
  result_data = NULL;
}
//++++++++++++++++++++++++++++++++++++++++++++
// custom methods
//---------------------------
bool jfFFNCrossoverSectionParser::ParsePageCore(size_t pageindex) {
  // constants
  const QString fname = "jfFFNCrossoverSectionParser::ParsePageCore";
  const QString arrow_tag = "<img src='/static/fcons/arrow-switch.png'";
  // variables
  QString buffer,errmsg;
  QString secname;
  bool addsucc;
  jfFFN_HalfCrossover* tcat;

  // starting
  if (!xparser.MovePast(arrow_tag)) return parsErr("Cannot find arrow icon!");
  if (!xparser.GetDelimitedFromEnd(">","Crossovers",buffer,"<hr size=1")) {
    return parsErr("Failed in extracting the Crossover Section name!");
  }
  secname = buffer.trimmed();
  // adding the plural
  if ((secname!="Misc") && (secname!="Anime/Manga")) secname += "s";
  // preparing for the list
  if (!xparser.MovePast("<div id='list_output'>")) return parsErr("Cannot find section crossover list!");
  if (!xparser.ChopAfter("</TD></TR></TABLE>",true)) return parsErr("Cannot find end of section crossover list!");

  // creating the result!
  result_data = new jfFFN_CrossoverSection();
  result_data->SetName(secname);

  // looping and getting the crossovers!
  while (xparser.GetMovePast("</div>",buffer)) {
    // creating the category
    tcat = new jfFFN_HalfCrossover();
    tcat->SetFromSource(secname,buffer,errmsg);
    if (!(tcat->IsValid())) {
      delete result_data;
      result_data = NULL;
      delete tcat;
      return parsErr("A crossover failed to parse : " + errmsg + " : " + buffer);
    }
    // adding the category...
    addsucc = result_data->AddHCrossCategory(tcat);
    if (!addsucc) {
      errmsg = "Problem adding category '" + buffer + "' at ";
      errmsg += QString::number((result_data->CatCount())+1);
      delete tcat;
      delete result_data;
      result_data = NULL;
      return parsErr(errmsg);
    }
    // done with the cat
  }
  // done with the loop
  return true;
}

//---------------------------
void* jfFFNCrossoverSectionParser::getResults() {
  return result_data;
}
//========================================================================
// --- { METHODS for jfFFNCrossoverParser] ------------------------------
//++++++++++++++++++++++++++++++++++++++++++++
// constructor
jfFFNCrossoverParser::jfFFNCrossoverParser():jfFFNCatParserBase() {
  result_data = NULL;
  crossstore_ptr=NULL;
}
//---------------------------
bool jfFFNCrossoverParser::SetCrossoverStoragePointer(jfFFN_CrossCatStore* in_cross_ptr) {
  if (in_cross_ptr == NULL) return false;
  crossstore_ptr = in_cross_ptr;
  return true;
}
//---------------------------
bool jfFFNCrossoverParser::SetHoldingSectionName(const QString& sname_in) {
  if (sname_in.isEmpty()) return false;
  sname = sname_in;
  return true;
}
//++++++++++++++++++++++++++++++++++++++++++++
// custom methods
//---------------------------
bool jfFFNCrossoverParser::ParsePageCore(size_t pageindex) {
  // constants
  const QString fname = "jfFFNCrossoverParser::ParsePageCore";
  const QString arrow_tag = "<img src='/static/fcons/arrow-switch.png'";
  // variables
  QString buffer, oerr;
  ulong uval;
  jfFFN_CrossoverCategory* tcat, *rcat;
  bool store_result;
  // we start...
  /**/lpt->tLog(fname,0);
  // assert(crossstore_ptr != NULL);
  /**/lpt->tLog(fname,1);
  if (!xparser.MovePast(arrow_tag)) return parsErr("Cannot Find Arrow Switch!");
  // catname and name, we use FromEnd just in case a category name includes 'Crossovers'
  if (!xparser.GetDelimitedFromEnd("absmiddle>","Crossovers",buffer,"<hr size=1")) {
    return parsErr("Failed in extracting the Crossover Category name!");
  }
  QString catname = buffer.trimmed();
  if (catname.isEmpty()) return parsErr("The crossover category name is Empty!");
  // getting the id
  if (!xparser.MovePast("<a class=btn href='/")) return parsErr("Cannot find main link!");
  if (!xparser.GetDelimitedULong("/","/",uval,oerr)) {
    return parsErr("Problems with getting the category ID : " + oerr);
  }
  // preparing for the list
  if (!xparser.MovePast("<div id='list_output'>")) return parsErr("Cannot find crossover list!");
  if (!xparser.ChopAfter("</TR></TABLE>",true)) return parsErr("Cannot find end of crossover list!");
  /**/lpt->tLog(fname,2);

  // we create the result here, and load basic info
  result_data = new jfFFN_CrossoverGroup();
  result_data->SetSectionName(sname);
  result_data->SetID(uval);
  result_data->SetCatName(catname);

  // looping and getting the crossovers!
  while (xparser.GetMovePast("</div>",buffer)) {
    // creating the category
    tcat = new jfFFN_CrossoverCategory();
    tcat->SetFromSource(catname,uval,buffer,oerr);
    if ((!tcat->IsValid()) && (!tcat->discard)) {
      delete tcat;
      /**/lpt->tLog(fname,3);
      delete result_data;
      result_data = NULL;
      return parsErr("A crossover failed to parse : " + oerr + " : " + buffer);
    }
    else if (tcat->discard) {
      delete tcat;
      /**/lpt->tLog(fname,4);
    }
    else {
      // adding the category...
      if (crossstore_ptr != NULL) {
        rcat = crossstore_ptr->AddCategoryAndSetSection(tcat,store_result,uval,sname);
        if (!store_result) delete tcat;
        result_data->AddCategoryWhenUnsorted(rcat);
      }
      else {
        tcat->SetSectionByID(uval,sname);
        result_data->AddCategoryWhenUnsorted(rcat);
      }

    }
    // done with the cat
  }
  /**/lpt->tLog(fname,5);
  // done
  return true;
}

//---------------------------
void* jfFFNCrossoverParser::getResults() {
  return result_data;
}
/******************************************************************************/
