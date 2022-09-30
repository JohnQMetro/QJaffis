/******************************************************************************
// Name:        categories.cpp
// Author :     John Q Metro
// Purpose :    Defines collections involving categorization
// Created:     April 1, 2009
// Conversion to QT started : September 5, 2013
// Updated:     March 27, 2022
******************************************************************************/
#ifndef CATEGORIES_H_INCLUDED
  #include "categories.h"
#endif // CATEGORIES_H_INCLUDED
#ifndef UTILS1_H_INCLUDED
  #include "../utils/utils1.h"
#endif // UTILS1_H_INCLUDED
#ifndef UTILS2_H_INCLUDED
  #include "../utils/utils2.h"
#endif // UTILS2_H_INCLUDED
#ifndef UTILS3_H_INCLUDED
#include "../utils/utils3.h"
#endif // UTILS3_H_INCLUDED
#ifndef LOGGING_H_INCLUDED
  #include "../utils/logging.h"
#endif // LOGGING_H_INCLUDED

#ifndef INITEND_H_INCLUDED
  #include "../../initend.h"
#endif // INITEND_H_INCLUDED


#include <assert.h>

#include <QDir>

//*****************************************************************************
// the defualt constructor
jfCategories::jfCategories() : jfBaseCollection() {
  size_t icount;
  for (icount=0;icount<32;icount++) {
    thecategories[icount]=NULL;
    fname_path[icount]=NULL;
    frelative[icount] = true;
  }
  localmap_ptr = NULL;
  // the default stuff
  usedefault = false;
  fname_path[32]=NULL;
  frelative[32] = true;  
  base_outputdir = jglobal::settings.paths.GetPathFor(jglobal::DEFAULT_PATH_TYPE::SAVED_RESULTS);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// creates an empty new filter as and makes it the last category
jfExpressionFilter* jfCategories::AddNewCategory(const QString& newname) {
  if (item_count==32) return NULL;
  thecategories[item_count] = new jfExpressionFilter();
  thecategories[item_count]->SetName(newname);
  thecategories[item_count]->SetFiltermapLink(localmap_ptr);
  fname_path[item_count] = new QString(newname+".html");
  frelative[item_count] = true;
  item_count++;
  return thecategories[item_count-1];
}
//-------------------------------------------------------------------------
// takes an existing expression filter and adds it as a category
bool jfCategories::AppendCategory(jfExpressionFilter* replacement) {
  if (item_count==32) return false;
  if (replacement==NULL) return false;
  // going ahead
  thecategories[item_count] = replacement;
  if (fname_path[item_count]==NULL) {
    fname_path[item_count] = new QString((replacement->GetName())+".html");
    frelative[item_count] = true;
  }
  // return true
  return true;
}
//------------------------------------------------------------------------
/* we might be treating what is leftover as a remainder catgeory. Filename
data for this is given index 33, but there is no expression. we use the
collections own name and description as the name and description for this
default category since jfCategories as alreays treated as part of a search
with no independant identity */
void jfCategories::SetUseDefault(bool tvalue) {
  if (tvalue) {
    if (fname_path[32]==NULL) {
      frelative[32]=true;
      fname_path[32] = new QString(GetName()+".html");
    }
  }
  usedefault = tvalue;
}
//-------------------------------------------------------------------------
bool jfCategories::SetLocalMapPtr(jfFilterMap* invalue) {
  localmap_ptr = invalue;
  for (size_t loopc=0; loopc<item_count; loopc++) {
    thecategories[loopc]->SetFiltermapLink(invalue);
  }
  return (invalue!=NULL);
}
//----------------------------------------------------------------
bool jfCategories::ChangeDefaultFName(QString in_name) {
	// cases where we return false;
  if (in_name.isEmpty()) return false;
	if (!usedefault) return false;
	// we produce a modified version for the file name
  if (fname_path[32]==NULL) fname_path[32] = new QString();
  (*fname_path[32]) = UBReplace(in_name,UFNAME_DELIMS)+".html";
	frelative[32] = true;
	// we update the name
  name = in_name + " ";
  if (item_count!=0) name += "Default ";
  name += "Results";
	// done
	return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// a special method called when starting a search
size_t jfCategories::CopyLoad() {
  for (size_t loopc=0; loopc<item_count; loopc++) {
    thecategories[loopc]->CopyLoad();
  }
  return item_count;
}
//------------------------------------------------------------------
bool jfCategories::VerifyNames(QString& omsg) {
  // variables
  bool qval;
  QString bufmsg, zmsg;
  jfNameVerif* vval;
  // startval
  vval = new jfNameVerif();
  qval = true;
  // the loop
  for (size_t loopc=0; loopc<item_count; loopc++) {
    qval = thecategories[loopc]->VerifyNames(vval,true);
    if (!qval) {
      bufmsg = " in category #" + QString::number(loopc);
      zmsg = vval->GetError();
      omsg = zmsg + bufmsg;
      break;
    }
  }
  delete vval;
  return qval;;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfCategories::UsesDefault() const {
  return usedefault;
}
//------------------------------------------------------------------------
bool jfCategories::NotEmpty() const {
  return (UsesDefault() || (!isEmpty()));
}
//-----------------------------------------------------------------------
bool jfCategories::ChIndex(const size_t& catval) const {
  if (catval==32) return UsesDefault();
  else if (catval<32) {
    return (catval<item_count);
  }
  return false;
}
//-------------------------------------------------------------------------
// we do sorting
bool jfCategories::Sort(jfBasePD* testee){
  size_t loopc;
  bool matched;
  // first, checks and special cases
  assert(testee!=NULL);
  if (item_count==0) return false;
  // the main loop, which marks the test elements for each matched category
  for (loopc=0;loopc<item_count;loopc++) {
    matched = thecategories[loopc]->Match(testee);
    (testee->mark)[loopc] = matched;
  }
  // the return value is true if anything has matched
  return testee->mark.any();
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// old virtual methods we implement
//--------------------------------------------------------------------------
bool jfCategories::NextIndex() {
  // special case, we are already off the end
  if (item_index==-2) return false;
  // the regular case
  if (item_index<(item_count-1)) {
    item_index++;
    curr_item = thecategories[item_index];
    return true;
  }
  // moving off the end
  else {
    item_index = -2;
    curr_item = NULL;
    return false;
  }
}
//--------------------------------------------------------------------------
bool jfCategories::BackIndex() {
  // case 1: we are already at the start
  if (item_index==-1) return false;
  // case 2: moving back from the start
  if (item_index==0) {
    item_index = -1;
    curr_item = NULL;
    return false;
  }
  // case 3 & 4
  if (item_index==-2) item_index=item_count-1;
  else item_index--;
  curr_item = thecategories[item_index];
  return true;
}
//--------------------------------------------------------------------------
void jfCategories::ResetIndex() {
  item_index = -1;
  curr_item = NULL;
}
//--------------------------------------------------------------------------
bool jfCategories::ToIndex(const int& newindex)  {
  // we check the index
  if (newindex>=item_count) return false;
  // things are okay
  item_index = newindex;
  curr_item = thecategories[item_index];
  return true;
}
//-------------------------------------------------------------------------
QString jfCategories::GetTypeID() const {
  return QString("jfCategories");
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// some more formerly abstract methods
//--------------------------------------------------------------------------
QStringList* jfCategories::GetNameList() const {
  QStringList* result;
  size_t loopindex;
  // for consistencey, we return NULL if the item count is 0
  if (item_count==0) return NULL;
  // not null
  result = new QStringList();
  for (loopindex=0;loopindex<item_count;loopindex++) {
    result->append(thecategories[loopindex]->GetName());
  }
  // done
  return result;
}
//--------------------------------------------------------------------------
QStringList* jfCategories::GetDescriptionList() const {
  QStringList* result;
  size_t loopindex;
  // for consistencey, we return NULL if the item count is 0
  if (item_count==0) return NULL;
  // not null
  result = new QStringList();
  for (loopindex=0;loopindex<item_count;loopindex++) {
    result->append(thecategories[loopindex]->GetDescription());
  }
  // done
  return result;
}
//--------------------------------------------------------------------------
QStringList* jfCategories::GetTypeList() const {
  QStringList* result;
  size_t loopindex;
  // for consistencey, we return NULL if the item count is 0
  if (item_count==0) return NULL;
  // not null
  result = new QStringList();
  for (loopindex=0;loopindex<item_count;loopindex++) {
    result->append(thecategories[loopindex]->GetTypeID());
  }
  // done
  return result;
}
//-----------------------------------------------------------------------
bool jfCategories::DeleteAtIndex(const size_t& dindex) {
  size_t lpc;
  if (dindex>=item_count) return false;
  // checking the indexes
  if (dindex==item_index) NextIndex();
  if (dindex==backup_index) {
    backup_item = NULL;
    backup_index = -1;
  }
  // deleting the item
  delete thecategories[dindex];
  thecategories[dindex] = NULL;
  delete fname_path[dindex];
  fname_path[dindex]=NULL;
  // moving the list
  for (lpc=(dindex+1);lpc<item_count;lpc++) {
    thecategories[lpc-1] = thecategories[lpc];
    fname_path[lpc-1] = fname_path[lpc];
  }
  thecategories[item_count-1] = NULL;
  fname_path[item_count-1] = NULL;
  frelative[item_count-1] = true;
  // finishing off
  item_count--;
  // resetting the indexes
  if (item_index>dindex) BackIndex();
  if (backup_index>dindex) {
    backup_index--;
    backup_item = thecategories[backup_index];
  }
  // done
  return true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfExpressionFilter* jfCategories::GetAtIndex(size_t gindex) {
  if (gindex>=32) return NULL;
  else return thecategories[gindex];
}
//----------------------------------------------------------------------
QString jfCategories::NameAtIndex(size_t gindex) const {
  if (gindex>=32) return "";
  return thecategories[gindex]->GetName();
}
//----------------------------------------------------------------------
QString jfCategories::DescAtIndex(size_t gindex) const {
  if (gindex>=32) return "";
  return thecategories[gindex]->GetDescription();
}
//-----------------------------------------------------------------------
QString jfCategories::PathAtIndex(size_t gindex) const {
    if (gindex>=item_count) {
      if ((gindex==32) && usedefault) return *(fname_path[32]);
      else return "";
    }
    else return *(fname_path[gindex]);
}
//-----------------------------------------------------------------------
bool jfCategories::RelativePathAtIndex(size_t gindex) const {
  if (gindex==32) {
    return frelative[32];
  }
  else {
    assert(gindex<item_count);
    return frelative[gindex];
  }
}
//------------------------------------------------------------------------
QString jfCategories::GetFullPath(size_t gindex) const {
  // the basic assertion
  assert((gindex<item_count)||(gindex==32));
  // variables
  QString rpath1,rpath2;
  // next up, absolute or relative
  if (frelative[gindex]) {
    rpath1 = *(fname_path[gindex]);
    rpath2 = base_outputdir + QDir::separator() + rpath1;
    rpath1 = QDir::cleanPath(rpath2);
    return rpath1;
  }
  else return *(fname_path[gindex]);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfCategories::ReplaceAtIndex(size_t gindex, jfExpressionFilter* replacement) {
  // checking the index
  assert(gindex<item_count);
  assert(replacement!=NULL);
  // replacing
  delete thecategories[gindex];
  thecategories[gindex] = replacement;
}
//------------------------------------------------------------------------
void jfCategories::SetFilePath(size_t gindex,const QString& src_fpath, bool relative) {
  if (gindex==32) assert(usedefault);
  else assert(gindex<item_count);
  // we start working on the replacement
  if (fname_path[gindex]==NULL) fname_path[gindex] = new QString(src_fpath);
  else (*fname_path[gindex]) = src_fpath;
  frelative[gindex] = relative;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the destructor
jfCategories::~jfCategories() {
  size_t lind;
  for (lind=0;lind<32;lind++) {
    if (thecategories[lind]!=NULL) delete thecategories[lind];
  }
  for (lind=0;lind<33;lind++) {
    if (fname_path[lind]!=NULL) delete fname_path[lind];
  }
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// here, we'll add default data
bool jfCategories::AddMid2ToFile(QTextStream* outfile) const {
  // variables
  jfOutString o_str;
  QString buffer;
  // the usual checks
  if (outfile==NULL) return false;
  // we build the result
  o_str << usedefault;
  o_str << ((usedefault)?(*fname_path[32]):(buffer));
  o_str << frelative[32];
  // adding the line
  (*outfile) << o_str << '\n';
  // adding the base output directory
  buffer = EscapeNL(base_outputdir,IO_DELIMS);
  buffer = ";BOD;" + buffer;
  (*outfile) << buffer << '\n';
  return true;
}
//----------------------------------------------------------------------------
// writes the expression to the file
bool jfCategories::AddRestToFile(QTextStream* outfile) const {
  size_t loopc;
  QString buffer;
  // the usual checks
  if (outfile==NULL) return false;
  // the loop
  for (loopc=0;loopc<item_count;loopc++) {
    // we prepare a line containing data not in the expression
    buffer = EscapeNL(*fname_path[loopc],IO_DELIMS);
    buffer +=';';
    buffer += (frelative[loopc])?("true"):("false");
    // then we add that line
    (*outfile) << buffer << '\n';
    // next, we output the expression
    thecategories[loopc]->AddToFile(outfile);
  }
  // done!
  return true;
}
//----------------------------------------------------------------------------
// here, we read the default category
bool jfCategories::ReadMid2FromFile(jfFileReader* infile) {
  // local variables
  const QString fname = "jfCategories::ReadMid2FromFile";
  QString buffer;
  QDir xdir;
  // we start
  assert(infile!=NULL);
  if (!infile->ReadParseLine(3,fname)) return false;
  // do we use default?
  if (!infile->lp.BoolVal(0,usedefault)) {
    return infile->BuildError("The first field is not a truth value!");
  }
  // we only do the next two steps if we *do* use default
  if (usedefault) {
    // getting the relative name+path string
    buffer = infile->lp.UnEscStr(1);
    fname_path[32] = new QString;
    (*fname_path[32]) = buffer;
    // getting the proper boolean value
    if (!infile->lp.BoolVal(2,frelative[32])) {
      return infile->BuildError("The relative field is not a truth value!");
    }
  }
  else {
    fname_path[32]=NULL;
    frelative[32]=false;
  }
  // next up, reading the base_outputdir (if it is there)
  if (!infile->ReadLine(buffer,fname)) return false;
  if (buffer.startsWith(";BOD;")) {
    buffer = buffer.mid(5);
    UnEscapeNL(buffer,IO_DELIMS);
    base_outputdir = buffer;
    xdir.setPath(base_outputdir);
    if (!xdir.exists()) {
        base_outputdir = jglobal::settings.paths.GetPathFor(jglobal::SAVED_RESULTS);
    }
  }
  else {
    infile->Backup();
  }
  // done
  return true;
}
//----------------------------------------------------------------------------
// reads it all from the file
bool jfCategories::ReadRestFromFile(jfFileReader* infile) {
  const QString fname = "jfCategories::ReadRestFromFile";
  // local variables
  size_t loopc;
  QString buffer;
  bool rres;
  // starting
  // the reading loop
  for (loopc=0;loopc<item_count;loopc++) {
    // getting the line
    if (!infile->ReadParseLine(2,fname)) return false;
    // now we check the fields
    buffer = infile->lp.UnEscStr(0);
    fname_path[loopc] = new QString();
    (*fname_path[loopc]) = buffer;
    if (!infile->lp.BoolVal(1,frelative[loopc])) {
      return infile->BuildError("This field is not a truth value!");
    }
    // reading in...
    thecategories[loopc] = new jfExpressionFilter();
    thecategories[loopc]->SetFiltermapLink(localmap_ptr);
    rres = thecategories[loopc]->GetFromFile(infile);
    if (!rres) {
      delete thecategories[loopc];
      return false;
    }
  }
  // done!
  return true;
}
//*****************************************************************************
