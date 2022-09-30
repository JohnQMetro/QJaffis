/******************************************************************************
Name    :   ao3_coll.cpp
Author  :   John Q Metro
Purpose :   Defines category results collection for AO3
Created :   September 1, 2012
Conversion to Qt Started March 29, 2014
Updated :   April 3, 2022
******************************************************************************/
#ifndef AO3_COLL_H_INCLUDED
  #include "ao3_coll.h"
#endif // AO3_COLL_H_INCLUDED
#ifndef SKELCOLL_H_INCLUDED
  #include "../../../core/structs/skelcoll.h"
#endif // SKELCOLL_H_INCLUDED
//--------------------------------
#include <math.h>
//-----------------------------------
// alternate constructor for use when loading data from file
jfAO3_FicList::jfAO3_FicList():jfTypedCollection<jfAO3Fanfic>() {
  cat_link = NULL;

}
//-------------------------------------------------------------------------------
jfAO3_FicList::jfAO3_FicList(size_t src_id, const QString& nname):jfTypedCollection<jfAO3Fanfic>(src_id,nname) {
  cat_link = NULL;
}

//-------------------------------------------------------------------------------
// the usual constructor
jfAO3_FicList::jfAO3_FicList(const jfAO3_Category* cat_linkin):jfTypedCollection<jfAO3Fanfic>() {
  // constants
  const QString fname = "jfAO3_FicList::jfAO3_FicList";
  // checks
  /**/jerror::AssertLog(cat_linkin!=NULL,fname,"The input category link is NULL!");
  /*jerror::AssertLog(cat_linkin!=NULL,fname,"The input category link is NULL!");*/
  // category link
  cat_link = cat_linkin;
  caturl = cat_link->GetUrl();
  cfinder = cat_link->GetCombo();
  SetName(cat_link->GetName() + " Fandom Fic Listing");
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented page methods
//-----------------------------------------------------
QString jfAO3_FicList::GetTypeID() const {
  return "AO3_Ficlist";
}
//-----------------------------------------------------
bool jfAO3_FicList::LoadValues(jfSkeletonParser* inparser,size_t which) const {
  LoadCoreValues(inparser,which);
  inparser->AddText("CATF_MAINLINK",cat_link->GetUrl());
  inparser->AddText("CATF_PERCENT",PercRCategory(which));
  return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// category manipulation methods
//-----------------------------------------------------
QString jfAO3_FicList::GetCatUrl() const {
  return cat_link->GetUrl();
}
//-----------------------------------------------------
QString jfAO3_FicList::GetFinder() const {
  return cfinder;
}
//----------------------------------------------------------------
const jfAO3_Category* jfAO3_FicList::GetCat() const {
  return cat_link;
}
//-----------------------------------------------------------
QString jfAO3_FicList::GetCatName() const {
  if (cat_link==NULL) return "";
  else return cat_link->GetName();
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfAO3_FicList::ReplaceCat(const jfAO3_Category* cat_linkin) {
  // constants
  const QString fname = "jfAO3_FicList::ReplaceCat";
  // a check
  assert(cat_linkin!=NULL);
  // replacing the link for the collection
  cat_link = cat_linkin;
  caturl = cat_link->GetUrl();
  cfinder = cat_link->GetCombo();
  // done
  return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// getting more information
jfFicExtract* jfAO3_FicList::FicExt_AtIndex(size_t i_index) const {
  if (i_index>=item_count) return NULL;
  else return dynamic_cast<jfAO3Fanfic*>(mainlist[i_index])->GetExtract();
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// some more virtual i/o methods
//-----------------------------------------------------
bool jfAO3_FicList::AddDelta(QTextStream* outfile) const {
  // variables
  QString hiid_str;
  // the usual checks
  if (outfile==NULL) return false;
  // the url must be used after loading to find the category object
  (*outfile) << caturl << "\n";
  (*outfile) << cfinder << "\n";
  // done
  return true;
}
//-----------------------------------------------------
bool jfAO3_FicList::ReadDelta(jfFileReader* infile) {
  const QString funcname = "jfAO3_FicList::ReadDelta";
  // starting with line 4
  assert(infile!=NULL);
  if (!infile->ReadLine(caturl,funcname)) return false;
  if (!infile->ReadLine(cfinder,funcname)) return false;
  return true;
}
//****************************************************************************
// the constructor
jfAO3ResColl::jfAO3ResColl(jfAO3Search* xsearch_in):jfResultMultiCollection(xsearch_in,true) {
  typed_search = xsearch_in;
  cindex = 0;
  cat_links = NULL;
}
//---------------------------------------------------------------------------
bool jfAO3ResColl::SetCatLinks(const jfGen_CatHolder* inlinks) {
  if (inlinks==NULL) return false;
  cat_links = inlinks;
  return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QString jfAO3ResColl::GetTypeID() const {
  return "jfAO3ResColl";
}
//--------------------------------------------------------------------------
const jfAO3_Category* jfAO3ResColl::GetCat(size_t zindex) const {
  // variables and checks
  jfAO3_FicList* temp;
  assert(CheckIndex(zindex));
  // going ahead
  temp = dynamic_cast<jfAO3_FicList*>((*collections)[zindex]);
  return temp->GetCat();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// i/o
bool jfAO3ResColl::WriteToHTML(size_t result_category) {
  // constants
  const QString fname = "jfAO3ResColl::WriteToHTML";
  // local variables
  jfHtmlParams *output;
  jfAO3_FicList* temp;
  size_t ccloop;
  size_t numres;
  // checks
  assert(result_category<33);
  // starting
  output = OpenHTMLOutfile(result_category);
  // we prepare the skeletop
  output->base = search->GetLocalSkeleton();
  if ((output->base)==NULL) {
    output->base = mainskelc->AtIndexM(4);
  }
  // we load up with values first
  search->LoadValues(output->parse,output->GetResCat());
  // header
  WriteHtmlHeader(output);
  // body
  for (ccloop=0; ccloop<itemcount; ccloop++) {
    temp = dynamic_cast<jfAO3_FicList*>((*collections)[ccloop]);
    numres = temp->CountRCategory(output->GetResCat());
    if (numres>0) temp->WriteToHTML(output,false);
  }
  // finishing off
  WriteFooterClose(output);
  return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// gets the fic extract (an abstract of the fic data used for fic downloading)
jfFicExtract* jfAO3ResColl::GetFicExtract(size_t c_index, size_t i_index) const {
  // constants
  const QString fname = "jfAO3ResColl::GetFicExtract";
  // checking the collections...
  /**/JDEBUGLOGST(fname,1,c_index)
  /**/JDEBUGLOGST(fname,2,itemcount)
  assert(c_index<itemcount);
  // variables
  jfAO3_FicList* typox;
  // starting
  (*cprotect)[c_index]->lock();
  typox = dynamic_cast<jfAO3_FicList*>((*collections)[c_index]);
  // getting the data
  jfFicExtract* xres = typox->FicExt_AtIndex(i_index);
  (*cprotect)[c_index]->unlock();
  // done
  return xres;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfAO3ResColl::WriteHtmlHeader(jfHtmlParams* indata) {
  // constants
  const QString fname = "jfAO3ResColl::WriteHtmlHeader";
  // local variables
  QString buffer;
  size_t ccloop;
  size_t numres;
  bool firstdone;
  bool lvres;
  QString skeldata;
  jfAO3_FicList* temp;
  // the usual checks
  jerror::AssertLog(indata->CheckOk(),fname,indata->errmsg);
  // loading values
  // the start of the header
  buffer = indata->MakeResult(indata->base->header_skel);
  (*(indata->outfile)) << buffer << "\n";
  firstdone = false;
  // category links
  for (ccloop=0; ccloop<itemcount; ccloop++) {
    // getting the collection and loading the values...
    temp = dynamic_cast<jfAO3_FicList*>((*collections)[ccloop]);
    lvres = temp->LoadValues(indata->parse,indata->GetResCat());
    assert(lvres);
    numres = temp->CountRCategory(indata->GetResCat());
    // the cat link
    if (numres>0) {
      // the cat separator
      if (firstdone) {
        skeldata = indata->Multiple()->catlink_separator;
        buffer = indata->MakeResult(skeldata);
        buffer += " ";
      }
      skeldata = indata->Multiple()->catlink;
      if (firstdone) buffer += indata->MakeResult(skeldata);
      else buffer = indata->MakeResult(skeldata);
      // writing the result
      (*(indata->outfile)) << buffer << "\n";
      firstdone = true;
    }
  }
  // then we have a footer below...
  skeldata = indata->Multiple()->top_footer;
  buffer = indata->MakeResult(skeldata);
  (*(indata->outfile)) << buffer << "\n";
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// i/o methods
//----------------------------
// we do not actually have anything extra to add
bool jfAO3ResColl::AddRestToFile(QTextStream* outfile) const {
  return true;
}
//----------------------------
// we create an empty Fanfiction.Net Collection
jfUrlItemCollection* jfAO3ResColl::MakeEmptyCollection() const {
  return new jfAO3_FicList();
}
//----------------------------
/* although no new items are to be loaded, we need to link each collection
with its appropriate category */
bool jfAO3ResColl::ReadRestFromFile(jfFileReader* infile) {
  // constants
  const QString fname = "jfAO3ResColl::ReadRestFromFile";
  // variables
  jfAO3_FicList* temp;
  const jfAO3_Category* cresult;
  size_t ccloop;
  QString cfindstr;
  bool dres;
  // a check
  /**/JDEBUGLOG(fname,1);
  assert(cat_links!=NULL);
  /**/JDEBUGLOG(fname,2);
  // we loop over each collection
  for (ccloop=0;ccloop<itemcount;ccloop++) {
    // getting the collection
    /**/JDEBUGLOGST(fname,3,ccloop);
    temp = dynamic_cast<jfAO3_FicList*>((*collections)[ccloop]);
    // looking for the cat link
    cfindstr = temp->GetFinder();
    cresult = dynamic_cast<const jfAO3_Category*>(cat_links->FindCatFromFinder(cfindstr));
    // a null result is badd
    if (cresult==NULL) {
      /**/JDEBUGLOGS(fname,4,cfindstr);
      // so therefore we delete the entire category
      dres = DeleteAtIndex(ccloop);
      assert(dres);
      ccloop--;
    }
    else {
      // if not null, we call the function thats set the cat
      temp->ReplaceCat(cresult);
    }
    /**/JDEBUGLOG(fname,5);
  }
  /**/JDEBUGLOG(fname,6);
  // done okay!
  return true;
}
//*****************************************************************************
