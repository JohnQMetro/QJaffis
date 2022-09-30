/******************************************************************************
Name    :   fim_groupsearch.cpp
Author  :   John Q Metro
Purpose :   Group Search Object for fimfiction.net
Created :   August 7, 2013
Started conversion to Qt August 3, 2015
Updated :   January 7, 2018 (Fimfiction.net has added group searching...)
******************************************************************************/
#ifndef FIM_GROUPSEARCH_H_INCLUDED
  #include "fim_groupsearch.h"
#endif // FIM_GROUPSEARCH_H_INCLUDED
#ifndef FIM_CONSTANTS_H
  #include "fim_constants.h"
#endif // FIM_CONSTANTS_H
#ifndef FIM_GROUPOBJ_H_INCLUDED
  #include "fim_groupobj.h"
#endif // FIM_GROUPOBJ_H_INCLUDED
#ifndef UTILS3_H_INCLUDED
  #include "../../../core/utils/utils3.h"
#endif // UTILS3_H_INCLUDED

#ifndef SKELCOLL_H_INCLUDED
  #include "../../../core/structs/skelcoll.h"
#endif // SKELCOLL_H_INCLUDED
//------------------------------------------
/*****************************************************************************/
// constructors
jfFIMGroupSearch::jfFIMGroupSearch():jfSearchCore() {
  pagelimit = 4000000;
  order = 0;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// setting data
//----------------------------
bool jfFIMGroupSearch::SetSearchString(const QString& valin, size_t inorder) {
    if (inorder >= fimcon::group_ordercount) return false;
    order = inorder;
    sstring = valin;
    esc_string = QueryEncode(valin);
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
  QString qres = "https://www.fimfiction.net/groups?q=";
  qres += esc_string;
  qres += "&order=" + fimcon::group_ordername[order];
  if (inp_index > 1) qres += "&page=" + QString::number(inp_index);
  return qres;
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
// returning data data
QString jfFIMGroupSearch::GetSearchString() const {
    return sstring;
}
//-------------------------------------------
size_t jfFIMGroupSearch::GetOrder() const {
    return order;
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
  resline << sstring << order;
  (*outfile) << resline << '\n';
  resline.FullClear();
  // done
  return true;
}
//-------------------------------------------------------
bool jfFIMGroupSearch::ReadMiddleFromFile(jfFileReader* infile) {
  const QString funcname = "jfFIMGroupSearch::ReadMiddleFromFile";
  // local variables
  assert(infile!=NULL);
  // line 1, word counts
  if (!infile->ReadParseLine(2,funcname)) return false;
  // getting the values
  sstring = (infile->lp).UnEscStr(0);
  esc_string = QueryEncode(sstring);
  QString oerr;
  if (!infile->lp.SBoundVal(1,3,order,oerr)) {
      return infile->BuildError("Search Results Order Wrong: " + oerr);
  }
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
