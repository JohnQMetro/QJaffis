/******************************************************************************
Name    :   fim_groupsearch.cpp
Author  :   John Q Metro
Purpose :   Group Search Object for fimfiction.net
Created :   August 7, 2013
Started conversion to Qt August 3, 2015
Updated :   June 8, 2017 (Fimfiction.Net 4)
******************************************************************************/
#ifndef FIM_GROUPSEARCH_H_INCLUDED
  #include "fim_groupsearch.h"
#endif // FIM_GROUPSEARCH_H_INCLUDED
#ifndef FIM_GROUPOBJ_H_INCLUDED
  #include "fim_groupobj.h"
#endif // FIM_GROUPOBJ_H_INCLUDED
#ifndef INITEND_H_INCLUDED
  #include "../../../initend.h"
#endif // INITEND_H_INCLUDED

#ifndef SKELCOLL_H_INCLUDED
  #include "../../../core/structs/skelcoll.h"
#endif // SKELCOLL_H_INCLUDED
//------------------------------------------
/*****************************************************************************/
// constructors
jfFIMGroupSearch::jfFIMGroupSearch():jfSearchCore() {
  pagelimit = 4000000;
  min_group1 = jglobal::settings.fimlimits[1];
  max_group1 = jglobal::settings.fimlimits[2];
  min_group2 = jglobal::settings.fimlimits[3];
  max_group2 = jglobal::settings.fimlimits[4];
  nofirst = false;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// setting data
//----------------------------
bool jfFIMGroupSearch::SetLimits(bool first, size_t fmin_in,size_t fmax_in) {
  if (fmax_in < fmin_in) return false;
  if (fmax_in > pagelimit) return false;
  if (first) {
    min_group1 = fmin_in;
    max_group1 = fmax_in;
    nofirst = ((min_group1==0) && (max_group1==0));
  }
  else {
    min_group2 = fmin_in;
    max_group2 = fmax_in;
  }
  return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented method for sending category data
void jfFIMGroupSearch::DispatchCategory() {
  // variables
  struct jfCategoryInfoMsg oval;
  // building the event to send
  oval.catheader = GetCatHeader();
  oval.catname = GetCatName();
  emit SendCatInfo(oval);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemeted virtual methods
//----------------------------
QString jfFIMGroupSearch::GetTypeID() const {
  return "FIMGroupSearch";
}
//----------------------------
QString jfFIMGroupSearch::GetCatName() const {
  return "FIMGroup";
}
//----------------------------
QString jfFIMGroupSearch::GetCatHeader() const {
  return "";
}
//----------------------------
QString jfFIMGroupSearch::GetIndexUrl(size_t inp_index) const {
  QString qres = "https://www.fimfiction.net/group/";
  return (qres + QString::number(inp_index)) + "/";
}
//----------------------------
QString jfFIMGroupSearch::GetHTMLHeader(size_t result_category) const  {
  QString result;
  result = "<h1 class=\"maintittle\">FIMFiction.Net Group Search Results</h1>\n";
  result += "<h2 class=\"res_categ_header\">Result Category : ";
  if (result_category == 32) result += categories->GetName();
  else result += categories->NameAtIndex(result_category);
  result += "</h2>";
  return result;
}
//----------------------------
void jfFIMGroupSearch::MakeLocalSkeleton(jfSkeletonBase* src) {
  jfSkeletonSingle* temp, *newval;
  // asserts and checks
  assert(src!=NULL);
  assert(src->GetSkelType()==jfs_SINGLE);
  // we start
  temp = dynamic_cast<jfSkeletonSingle*>(src);
  newval = new jfSkeletonSingle(temp);
  // clearing
  if (local_skel) ClearLocalSkeleton();
  skel_link = newval;
  local_skel = true;
}
//----------------------------
void jfFIMGroupSearch::LoadValues(jfSkeletonParser* inparser, size_t result_categ) {
  LoadCoreValues(inparser,result_categ);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// returning data data
//-------------------------------------------
size_t jfFIMGroupSearch::GetMinGroup(bool first) const {
  return (first)?(min_group1):(min_group2);
}
//-------------------------------------------
size_t jfFIMGroupSearch::GetMaxGroup(bool first) const {
  return (first)?(max_group1):(max_group2);
}
//----------------------------------------------
bool jfFIMGroupSearch::NoFirst() const {
  return nofirst;
}
//----------------------------------------------
size_t jfFIMGroupSearch::GroupsToSearch(bool first) const {
  if (first) {
    if (nofirst) return 0;
    else return (max_group1-min_group1)+1;
  }
  else return (max_group2-min_group2)+1;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// i/o methods for google specific data
//-----------------------------------------------
bool jfFIMGroupSearch::AddMiddleToFile(QTextStream* outfile) const {
  // local variables
  jfOutString resline;
  // checking and special conditions
  if (outfile==NULL) return false;
  // building the first output line
  resline << min_group1 << max_group1 << min_group2 << max_group2;
  (*outfile) << resline << '\n';
  resline.clear();
  // done
  return true;
}
//-------------------------------------------------------
bool jfFIMGroupSearch::ReadMiddleFromFile(jfFileReader* infile) {
  const QString funcname = "jfFIMGroupSearch::ReadMiddleFromFile";
  // local variables
  QString buffer;
  assert(infile!=NULL);
  // line 1, word counts
  if (!infile->ReadParseLine(4,funcname)) return false;
  // getting the values
  if (!infile->lp.SIntVal(0,min_group1)) return infile->BuildError("The minimum group index 1 is not a number!");
  if (!infile->lp.SIntVal(1,max_group1)) return infile->BuildError("The maximum group index 1 is not a number!");
  if (!infile->lp.SIntVal(2,min_group2)) return infile->BuildError("The minimum group index 2 is not a number!");
  if (!infile->lp.SIntVal(3,max_group2)) return infile->BuildError("The maximum group index 2 is not a number!");
  if (max_group1<min_group1) return infile->BuildError("The max index group is less than the min group index!");
  nofirst = ((min_group1==0) && (max_group1==0));
  // done
  return true;
}
//=============================================================================
// constructor
jfFIMGroupResColl::jfFIMGroupResColl(jfFIMGroupSearch* xsearch_in):jfResultCollection(xsearch_in, false) {
  typed_search = xsearch_in;
  jfUrlItemCollection* xc = MakeEmptyCollection();
  xc->SetID(1);
  xc->SetName("The Only Collection!");
  AddItem(xc);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QString jfFIMGroupResColl::GetTypeID() const {
  return "FIMGroupResColl";
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfFIMGroupResColl::WriteToHTML(size_t result_category) {
  // local variables
  jfHtmlParams *output;
  jfFIMGroupCollection* temp;
  // checks
  assert(result_category<33);
  // starting
  output = OpenHTMLOutfile(result_category);
  // we prepare the skeletop
  output->base = search->GetLocalSkeleton();
  if ((output->base)==NULL) {
    output->base = mainskelc->AtIndexS(5);
  }
  // we load up with values first
  search->LoadValues(output->parse,output->GetResCat());
  // header
  WriteHtmlHeader(output);
  // body
  temp = dynamic_cast<jfFIMGroupCollection*>((*collections)[0]);
  temp->WriteToHTML(output,true);
  // finishing off
  WriteFooterClose(output);
  return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfFIMGroupResColl::WriteHtmlHeader(jfHtmlParams* indata) {
  // local variables
  QString buffer;
  // the usual checks
  assert(indata!=NULL);
  assert(indata->CheckOk());
  // loading values
  // constatnts
  buffer = indata->MakeResult(indata->base->header_skel);
  (*(indata->outfile)) << buffer << '\n';
}
//-------------------------------------------------------------------------
jfUrlItemCollection* jfFIMGroupResColl::MakeEmptyCollection() const {
  return new jfFIMGroupCollection();
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented private i/o methods
//--------------------------------------
bool jfFIMGroupResColl::AddRestToFile(QTextStream* outfile) const {
  // does nothing
  return true;
}
//--------------------------------------
bool jfFIMGroupResColl::ReadRestFromFile(jfFileReader* infile) {
  return true;
}
/*****************************************************************************/
