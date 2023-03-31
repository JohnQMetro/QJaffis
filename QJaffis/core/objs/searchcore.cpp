/*****************************************************************************
Name    :   searchcore.cpp
Author  :   John Q Metro
Purpose :   Declares a base class containing search info and parameters
Created :   May 4, 2009
Conversion to Qt started November 5, 2013
Updated :   April 30, 2014
******************************************************************************/
#ifndef SEARCHCORE_H_INCLUDED
  #include "searchcore.h"
#endif // SEARCHCORE_H_INCLUDED
#ifndef UTILS1_H_INCLUDED
  #include "../utils/utils1.h"
#endif // UTILS1_H_INCLUDED
#ifndef LOGGING_H_INCLUDED
  #include "../utils/logging.h"
#endif // LOGGING_H_INCLUDED
//--------------------------------------
#include <assert.h>
/*****************************************************************************/
// the constructor, just makes sure links are NULL
jfSearchCore::jfSearchCore() {
  def_filtermap = NULL;
  local_map = NULL;
  categories = NULL;
  skel_link = NULL;
  local_skel = false;
  catindex = 0;
}
jfSearchCore::~jfSearchCore() {}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// checks the startup set for validity
bool jfSearchCore::Check() const {
  if (categories==NULL) return false;
  if (def_filtermap==NULL) return false;
  if (local_map==NULL) return false;
  return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfSearchCore::NoFilters() const {
    bool emptyDef;
    bool emptyLocal;
    if (def_filtermap == NULL) emptyDef = true;
    else emptyDef = def_filtermap->isEmpty();
    if (local_map == NULL) emptyLocal = true;
    else emptyLocal = local_map->isEmpty();
    return (emptyDef && emptyLocal);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// makes a set of empty maps and categories
void jfSearchCore::MakeEmpty() {
  if (def_filtermap==NULL) def_filtermap = new jfFilterMap();
  if (local_map==NULL) local_map = new jfFilterMap();
  if (categories==NULL) categories = new jfCategories();
  categories->SetLocalMapPtr(local_map);
}
//---------------------------------------------------------------------
size_t jfSearchCore::GetPagelimit() const {
  return pagelimit;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// copy load (called just before search)
void jfSearchCore::CopyLoad() {
  // checks
  assert(local_map!=NULL);
  assert(def_filtermap!=NULL);
  assert(categories!=NULL);
  // calling CopyLoad on subparts
  def_filtermap->CopyLoad();
  categories->CopyLoad();
}
//--------------------------------------------------------------
bool jfSearchCore::VerifyNames(QString&outmsg) {
  // constants
  const QString fname = "jfSearchCore::VerifyNames";
  // checks
  assert(local_map!=NULL);
  assert(def_filtermap!=NULL);
  assert(categories!=NULL);
  // variables
  QString omsg;
  bool rval;
  // calling CopyLoad on subparts
  /**/JDEBUGLOGB(fname,1,def_filtermap==NULL)
  /**/JDEBUGLOGST(fname,2,def_filtermap->GetCount())
  rval = def_filtermap->VerifyNames(omsg,true);
  if (!rval) {
    outmsg = omsg + " in included filters.";
    return false;
  }
  /**/JDEBUGLOG(fname,3);
  // calling CopyLoad on subparts
  rval = categories->VerifyNames(omsg);
  if (!rval) {
    outmsg = omsg + " in result categories.";
    /**/JDEBUGLOGS(fname,4,outmsg)
    return false;
  }
  else return true;
}
//--------------------------------------------------------------------
// initializes/or writes logging for pair filters
void jfSearchCore::PairFilterLogging(bool write) {
  // checks
  assert(local_map!=NULL);
  assert(def_filtermap!=NULL);
  assert(categories!=NULL);
  // calling CopyLoad on subparts
  local_map->PairFilterLogging(write);
  def_filtermap->PairFilterLogging(write);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// dispatches the event containing result data (to the organizer)
void jfSearchCore::DispatchResults(size_t stotal,jfResultUnitVector* inresult) {
  // checking
  assert(Check());
  assert(inresult!=NULL);
  assert(stotal>=(inresult->size()));
  assert((inresult->size())>0);
  // setting the value of the event
  inresult->stotal = stotal;
  // we dispatch the result to the organizer
  emit SendResults(inresult);
}
//-------------------------------------------------------------------
void jfSearchCore::SendNewcount(size_t icount) {
  emit DispatchCount(icount);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/* if we do not use the default (leftover) result category, we return
TRUE if the paramster has no category marks */
bool jfSearchCore::NoCatTest(const jfItemMetaFlags* testee) const {
    assert(testee!=NULL);
    assert(categories!=NULL);
    if (!(categories->UsesDefault())) {
        if ((testee->category).any()) return false;
        else return true;
    }
    else return false;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfSearchCore::ClearLocalSkeleton() {
  if (local_skel) {
    assert(skel_link!=NULL);
    delete skel_link;
    skel_link = NULL;
    local_skel = false;
  }
}
//-------------------------------------------------------------------------
jfSkeletonBase* jfSearchCore::GetLocalSkeleton() {
  return skel_link;
};
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// loading up data...
void jfSearchCore::LoadCoreValues(jfSkeletonParser* inparser, size_t result_categ) const {
  // checks
  assert(inparser!=NULL);
  assert(Check());
  assert(categories->ChIndex(result_categ));
  // we start to load values
  inparser->AddText("SEARCH_NAME",name);
  inparser->AddText("SEARCH_DESCRIPTION",description);
  inparser->AddUInt("SEARCH_ID",num_id);
  // result category data
  if (result_categ==32) {
    inparser->AddText("RCATEG_NAME",categories->GetName());
    inparser->AddText("RCATEG_DESCRIPTION",categories->GetDescription());
  }
  else {
    inparser->AddText("RCATEG_NAME",categories->NameAtIndex(result_categ));
    inparser->AddText("RCATEG_DESCRIPTION",categories->DescAtIndex(result_categ));
  }

}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  implemented virtual i/o method
//----------------------------------------------------------------------
bool jfSearchCore::AddRestToFile(QTextStream* outfile) const {
  QString buffer;
  // the usual checks
  if (outfile==NULL) return false;
  // adding a flag
  buffer = Bool2Str(local_skel);
  buffer += ';';
  if (local_skel) buffer += Bool2Str((skel_link->GetSkelType()==jfs_MULTIPLE));
  else buffer += "false";
  (*outfile) << buffer << "\n";
  // appending the subparts
  if (local_skel) {
   if (!(skel_link->AddToFile(outfile))) return false;
  }
  if (!(local_map->AddToFile(outfile))) return false;
  if (!(def_filtermap->AddToFile(outfile))) return false;
  if (!(categories->AddToFile(outfile))) return false;
  // done
  return true;
}
//-------------------------------------------------------------------------
bool jfSearchCore::ReadRestFromFile(jfFileReader* infile) {
  const QString fname = "jfSearchCore::ReadRestFromFile";
  // local variables
  bool multiple;
  jfSkeletonSingle* temp1;
  jfSkeletonMultiple* temp2;
  // we read in a flag line for a local skeleton store
  /**/JDEBUGLOG(fname,1)
  assert(infile!=NULL);
  if (!infile->ReadParseLine(2,fname)) return false;
  if (!infile->lp.BoolVal(0,local_skel)) return infile->BuildError("Not a Boolean value!");
  if (!infile->lp.BoolVal(1,multiple)) return infile->BuildError("Not a Boolean value!");
  /**/JDEBUGLOG(fname,2)
  // clearing out old skeleton data
  if (skel_link != NULL ) {
    /**/JDEBUGLOG(fname,3)
    delete skel_link;
    skel_link = NULL;
  }
  /**/JDEBUGLOG(fname,4)
  // next, we do read in the store if need be
  if (local_skel) {
    // we read in the skeleton store
    if (!multiple) {
      temp1 = new jfSkeletonSingle();
      if (!temp1->GetFromFile(infile)) {
        delete temp1;
        return false;
      }
      skel_link = temp1;
    }
    else {
      temp2 = new jfSkeletonMultiple();
      if (!temp2->GetFromFile(infile)) {
        delete temp2;
        return false;
      }
      skel_link = temp2;
    }
  }
  /**/JDEBUGLOG(fname,5)
  // local filters
  // creating the local filter map
  if (local_map!=NULL) delete local_map;
  local_map = new jfFilterMap();
  /**/JDEBUGLOG(fname,6)
  local_map->SetMapPtr(local_map);
  if (!local_map->GetFromFile(infile)) return false;
  /**/JDEBUGLOG(fname,7)
  // built-in filters
  if (def_filtermap!=NULL) delete def_filtermap;
  /**/JDEBUGLOG(fname,8)
  // the built-in filters
  def_filtermap = new jfFilterMap();
  def_filtermap->SetMapPtr(local_map);
  if (!def_filtermap->GetFromFile(infile)) return false;
  /**/JDEBUGLOG(fname,9)
  // the categories
  /**/JDEBUGLOG(fname,10)
  if (categories!=NULL) delete categories;
  categories = new jfCategories();
  categories->SetLocalMapPtr(local_map);
  if (!categories->GetFromFile(infile)) return false;
  /**/JDEBUGLOG(fname,11)
  return true;
}
/*****************************************************************************/
