/*****************************************************************************
Name:        itemcollect.cpp
Author :     John Q Metro
Purpose :    a vector collection base for jfBasePD that is used for storing
              and processitng items.
Created:     April 17, 2009
Conversion to Qt Started November 9, 2013
Updated:     June 21, 2016
******************************************************************************/
#ifndef ITEMCOLLECT_H_INCLUDED
  #include "itemcollect.h"
#endif // ITEMCOLLECT_H_INCLUDED
#ifndef UTILS1_H_INCLUDED
  #include "../utils/utils1.h"
#endif // UTILS1_H_INCLUDED
#ifndef UTILS2_H_INCLUDED
  #include "../utils/utils2.h"
#endif // UTILS2_H_INCLUDED
#ifndef LOGGING_H_INCLUDED
  #include "../utils/logging.h"
#endif // LOGGING_H_INCLUDED
//--------------------------------------
#include <assert.h>
/*****************************************************************************/
// constructor to initialize values
jfUrlItemCollection::jfUrlItemCollection():jfBaseCollection() {
  // constants
  insert_index = 0;
  grand_total = 0;
  // other
  idstore = NULL;
}
//----------------------------------------------------------------------
jfUrlItemCollection::jfUrlItemCollection(size_t src_id, const QString& nname):jfBaseCollection(src_id,nname) {
  // constants
  insert_index = 0;
  grand_total = 0;
  // other
  idstore = NULL;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
size_t jfUrlItemCollection::AddToGrandTotal(size_t add_amount) {
  grand_total += add_amount;
  insert_index++;
  return grand_total;
}
//--------------------------------------------------------------
bool jfUrlItemCollection::SetIDFiltering(jfIDStore* instore) {
  idstore = instore;
  return (idstore!=NULL);
}
//------------------------------------------------------------
bool jfUrlItemCollection::hasIDFiltering() const {
  return (idstore != NULL);
}
//----------------------------------------------------------------------
/* Checks the id's of the objects in new_items against the idstore. If
 * already included, the objects are marked 'included = false' (since they
 * should not be added). Otherwise, the id's are added to the store. */
size_t jfUrlItemCollection::IDFilterMarkResults(jfResultUnitVector* new_items) {
  // trivial wrong cases
  if (idstore == NULL) return 0;
  if (new_items == NULL) return 0;
  if (new_items->size() == 0) return 0;
  /* We loop thru the items, checking against the list (we assume no duplicates
   *within* the input items. */
  jfBasePD* item_to_check = NULL;
  size_t filtered_count = 0;

  for (size_t item_index = 0 ; item_index < new_items->size() ; item_index++) {
    item_to_check = const_cast<jfBasePD*>((*new_items)[item_index]->Sourcelink());
    item_to_check->included = !(idstore->DiscardTest(item_to_check));
    if (!(item_to_check->included)) filtered_count++;
  }
  // done
  return filtered_count;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// getting some basic data
size_t jfUrlItemCollection::InsertIndex() const {
  return insert_index;
}
//-----------------------------------------------------------------------
size_t jfUrlItemCollection::GetGrandTotal() const {
  return grand_total;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfUrlItemCollection::WriteToHTML(jfHtmlParams* indata, bool single) const {
  // local variables
  QString result,intermed;
  QString buffer;
  bool lvres,thlres;
  // checks
  assert(indata!=NULL);
  assert(indata->CheckOk());
  // we now load the parser with values
  lvres = LoadValues(indata->parse,indata->GetResCat());
  assert(lvres);
  // doing the parsing
  buffer = (single)?(indata->Single()->cat_header):(indata->Multiple()->cat_header);
  result = indata->MakeResult(buffer);
  // writing
  indata->AppendLine(result);
  // producing the results
  thlres = ToHTMLList(indata,single);
  assert(thlres);
  // doing the parsing
  buffer = (single)?(indata->Single()->cat_footer):(indata->Multiple()->cat_footer);
  result = indata->MakeResult(buffer);
  // writing
  indata->AppendLine(result);
  // done
  return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented virtual i/o methods
//----------------------------------------------------------------------------
/* What we write here is page index, page count, and grand total. I've decided
*not* to include search core, old index, or intermediate info here */
bool jfUrlItemCollection::AddMid2ToFile(QTextStream* outfile) const {
  // local varibales
  jfOutString buffer;
  // the usual checks
  if (outfile==NULL) return false;
  // starting
  buffer << insert_index << grand_total;
  // doing
  (*outfile) << QString(buffer) << "\n";
  return true;
}
//-----------------------------------------------------------------------------
bool jfUrlItemCollection::ReadMid2FromFile(jfFileReader* infile) {
  const QString fname="jfUrlItemCollection::ReadMid2FromFile";
  const QString inai=" value is not an integer!";
  // local variables
  QString cline;
  // starting...
  assert(infile!=NULL);
  if (!infile->ReadParseLine(2,fname)) return false;
  // now that we have the parser line, we get
  if (!infile->lp.SIntVal(0,insert_index)) return infile->BuildError("The page index"+inai);
  if (!infile->lp.SIntVal(1,grand_total)) return infile->BuildError("The grand item total"+inai);
  return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// loading values
void jfUrlItemCollection::LoadCoreValues(jfSkeletonParser* inparser,size_t which) const {
  // a basic checks
  assert(inparser!=NULL);
  // adding items
  inparser->AddText("CAT_NAME",name);
  inparser->AddText("CAT_DESC",name);
  inparser->AddUInt("CAT_ID",num_id);
  inparser->AddUInt("ITEM_COUNT",item_count);
  inparser->AddUInt("ITEM_RCOUNT",CountRCategory(which));
  inparser->AddUInt("CAT_TOTALCOUNT",grand_total);
  inparser->AddUInt("CAT_PAGECOUNT",insert_index);
}
/*****************************************************************************/
