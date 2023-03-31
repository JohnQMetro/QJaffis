/*****************************************************************************
Name:        itemcollect.cpp
Author :     John Q Metro
Purpose :    a vector collection base for jfBasePD that is used for storing
              and processitng items.
Created:     April 17, 2009
Conversion to Qt Started November 9, 2013
Updated:     March 25, 2023
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

#ifndef DEFCOLORS_H
    #include "../../interface/defcolors.h"
#endif // DEFCOLORS_H
//--------------------------------------
#include <assert.h>
/*****************************************************************************/
jfSearchResultItemCollectionBase::jfSearchResultItemCollectionBase(QString&& in_name, size_t in_num_id) {
    name = in_name;
    num_id = in_num_id;
    // constants
    insert_index = 0;
    grand_total = 0;
    // other
    idstore = NULL;
}
// ---------------------------------------
jfSearchResultItemCollectionBase:: jfSearchResultItemCollectionBase(const QString& in_name, size_t in_num_id) {
    name = in_name;
    num_id = in_num_id;
    // constants
    insert_index = 0;
    grand_total = 0;
    // other
    idstore = NULL;
}
//+++++++++++++++++++++++++++++++++++++++++++
size_t jfSearchResultItemCollectionBase::InsertIndex() const {
    return insert_index;
}
// ----------------------------------
size_t jfSearchResultItemCollectionBase::GetCount() const {
    return mainlist.size();
}
//-------------------------------------
/* counts how many items are marked as belonging to a particular result
category. 32 counts as no category at all */
size_t jfSearchResultItemCollectionBase::CountResultCategory(size_t which) const {
    assert(which<33);
    // variables
    size_t rcount = 0;
    const jfItemMetaFlags* flags = NULL;

    // the main loop
    for (size_t lcount = 0 ; lcount < mainlist.size() ; lcount++) {
        flags = mainlist.at(lcount).flags;
        if (flags != NULL) {

            if (which == 32) {
                if (!(flags->category.any())) rcount++;
            }
            else {
                if ((flags->category).test(which)) rcount++;
            }
        }
        // if there is no flag info, it goes in the default result category
        else if (which == 32) {
            rcount++;
        }
    }
    // done
    return rcount;
}
// ------------------------------------------------
QString jfSearchResultItemCollectionBase::PercentResultCategory(size_t which) const {
    // variables
    size_t rtotal;
    QString resval;
    double pval;
    // we start
    assert(which<33);
    if (grand_total==0) resval = "0.00%";
    else {
        rtotal = CountResultCategory(which);
        pval = (double(rtotal))/(double(grand_total));
        resval = ToPercent(pval,2);
    }
    // done
    return resval;
}
//++++++++++++++++++++++++++++++++++++++++++++
// extracting info at the specified index
// -------------------------------------
const QString& jfSearchResultItemCollectionBase::TitleAtIndex(size_t i_index) const {
    assert(i_index < mainlist.size());
    return mainlist.at(i_index).item->GetTitle();
}
// -------------------------------------
const QString& jfSearchResultItemCollectionBase::SummaryAtIndex(size_t i_index) const {
    assert(i_index < mainlist.size());
    return mainlist.at(i_index).item->GetSummary();
}
// -------------------------------------
const QString& jfSearchResultItemCollectionBase::UrlAtIndex(size_t i_index) const {
    assert(i_index < mainlist.size());
    return mainlist.at(i_index).item->GetURL();
}
//++++++++++++++++++++++++++++++++++++++++++++
bool jfSearchResultItemCollectionBase::AddResults(jfResultUnitVector* new_items) {
  if (new_items == NULL) return false;
  const jfSearchResultItem* item = NULL;           // the search result item
  jfItemMetaFlags* flags = NULL;
  // adding to the grand total
  grand_total += new_items->stotal;
  // getting links to the items from the new_items list
  for (size_t cur_index = 0 ; cur_index < new_items->size() ; cur_index++) {
      item = (*new_items)[cur_index]->SourceLink();
      flags = (*new_items)[cur_index]->Flags();
      mainlist.push_back(jfItemFlagGroup{ item , flags} );
  }
  // done
  insert_index++;
  return true;
}
// ---------------------------------------------
jfResultUnitVector* jfSearchResultItemCollectionBase::GetContentsAsResults(jfResultItemOutputter* outputter) const {
    // variables
    jfItemFlagGroup curritem;
    // starting
    jfResultUnitVector* result = new jfResultUnitVector();
    // loop that builds up result
    for (size_t rindex = 0 ; rindex < mainlist.size() ; rindex++) {
        curritem = mainlist.at(rindex);
        outputter->SetItemSource(curritem.item, curritem.flags);
        jfResultUnit* unit = new jfResultUnit(curritem.item, curritem.flags, outputter->ToDisplayHTML());
        result->push_back(unit);
    }
    result->stotal = grand_total;
    return result;
}
//++++++++++++++++++++++++++++++++++++++++++++
// Id Filtering
//---------------------------
bool jfSearchResultItemCollectionBase::SetIDFiltering(jfIDStore* instore) {
    idstore = instore;
    return (idstore!=NULL);
}
//---------------------------
bool jfSearchResultItemCollectionBase::HasIDFiltering() const {
    return (idstore != NULL);
}
//---------------------------
size_t jfSearchResultItemCollectionBase::IDFilterMarkResults(jfResultUnitVector* new_items) {
    // trivial wrong cases
    if (idstore == NULL) return 0;
    if (new_items == NULL) return 0;
    if (new_items->size() == 0) return 0;
    /* We loop thru the items, checking against the list (we assume no duplicates
     *within* the input items. */
    size_t filtered_count = 0;

    for (size_t item_index = 0 ; item_index < new_items->size() ; item_index++) {
        jfResultUnit* result_item = (*new_items)[item_index];
        bool is_included = !(idstore->DiscardTest(result_item->SourceLink()));
        result_item->SetIncluded(is_included);
        if (!is_included) filtered_count++;
    }
    // done
    return filtered_count;
}
// +++++++++++++++++++++++++++++++++++++++++++
// grand total
//---------------------------
size_t jfSearchResultItemCollectionBase::AddToGrandTotal(size_t add_amount) {
    grand_total += add_amount;
    insert_index++;
    return grand_total;
}
//---------------------------
size_t jfSearchResultItemCollectionBase::GetGrandTotal() const {
    return grand_total;
}
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfSearchResultItemCollectionBase::WriteToHTML(jfHtmlParams* indata, bool single) const {
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
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// clearing out stuff (for destruction!)
void jfSearchResultItemCollectionBase::ClearContents(bool destroy_too) {
    // variables
    size_t rloop;
    // if we destroy, we do that first
    if (destroy_too) {
        for (rloop=0 ; rloop < mainlist.size() ; rloop++) {
            mainlist[rloop].Dispose();
        }
    }
    // getting rid of the list
    mainlist.clear();
    //getting rid of the indexes
    /*
    item_index = -1;
    curr_item = NULL;
    backup_index = -1;
    backup_item = NULL;
    */
    // resetting the counts
    grand_total = 0;
    insert_index = 0;
}
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfSearchResultItemCollectionBase::~jfSearchResultItemCollectionBase() {

}
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfSearchResultItemCollectionBase::ToHTMLList(jfHtmlParams* indata, bool single) const {
    // we start
    i_const_iterator findex = mainlist.begin();
    size_t numresults = 0;

    // the main loop
    while (findex<mainlist.end()) {
        bool use_it = true;
        const jfItemMetaFlags* cflags = findex->flags;

        // we check if we use the current item
        // if the result category is the default, none of the categories can be marked
        if (indata->GetResCat()==32) {
            bool any_category =(cflags != NULL) && (cflags->category.any());
            use_it = !any_category;
        }
        // otherwise, we check the propert category
        else {
            use_it = (cflags != NULL) && (cflags->category.test(indata->GetResCat()));
        }


        // we only do the next if use_it is true
        if (use_it) {
            QString tvalue, result;
            // we add a separator first (since there is no way to tell if there are any following items.
            if (numresults!=0) {
                tvalue = (single) ? (indata->Single()->item_separator) : (indata->Multiple()->item_separator);
                result = indata->MakeResult(tvalue);
                indata->AppendLine(result);
            }
            // getting and writing the html data for the item
            (findex->item)->LoadValues(indata->parse);
            tvalue = (single) ? (indata->Single()->item_skel) : (indata->Multiple()->item_skel);
            result = indata->MakeResult(tvalue);
            indata->AppendLine(result);
            // index increments
            numresults++;
        }
        findex++;
    }
    // done
    return true;
}
// --------------------------------------------------------------
void jfSearchResultItemCollectionBase::LoadCoreValues(jfSkeletonParser* inparser,size_t which) const {
    // a basic checks
    assert(inparser!=NULL);
    // adding items
    inparser->AddText("CAT_NAME",name);
    inparser->AddText("CAT_DESC",name);
    inparser->AddUInt("CAT_ID",num_id);
    inparser->AddUInt("ITEM_COUNT",mainlist.size());
    inparser->AddUInt("ITEM_RCOUNT",CountResultCategory(which));
    inparser->AddUInt("CAT_TOTALCOUNT",grand_total);
    inparser->AddUInt("CAT_PAGECOUNT",insert_index);
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfSearchResultItemCollectionBase::AddToFile(QTextStream* outfile) const {
    if (outfile==NULL) return false;
    // constants and variables
    const QString fname = "jfSearchResultItemCollectionBase::AddToFile";
    jfOutString buffer;

    // checking and special conditions
    // NOTE: more fields may be added in, like type, etc
    buffer << TypeId() << name << num_id << mainlist.size();
    (*outfile) << "### " << buffer << "\n";
    buffer.FullClear();
    buffer << insert_index << grand_total;
    // doing
    (*outfile) << QString(buffer) << "\n";

    // next, we call a method that must be implemented by any non-abstract descendant
    if (!AddDelta(outfile)) return false;

    size_t loopc;
    // writing the results loop
    for (loopc=0; loopc < mainlist.size(); loopc++) {
        if (!WriteGroupToFile(mainlist.at(loopc), outfile)) return false;
    }
    // adding on the footer
    (*outfile) << "###\n";
    return true;
}
// --------------------------------
bool jfSearchResultItemCollectionBase::GetFromFile(jfFileReader* infile) {
    const QString funcname="jfSearchResultItemCollectionBase::GetFromFile";
    // input data
    QString buffer;
    // starting checks
    assert(infile!=NULL);
    // Reading and processing line 1
    if (!infile->ReadParseSLine(4,funcname)) return false;
    buffer = infile->lp.UnEscStr(0);
    if (buffer != TypeId()) {
        return infile->BuildError("The type (" + buffer + ") does not match!");
    }
    // field 2 is the name
    name = infile->lp.UnEscStr(1);
    // field 3 is the numeric id
    if (!infile->lp.SIntVal(2, num_id)) {
        return infile->BuildError("The numeric is not an integer!");
    }
    // field two is the number of items
    size_t expected_item_count;
    if (!infile->lp.SIntVal(3,expected_item_count)) {
        return infile->BuildError("The expected item count is not an integer!");
    }

    // we get the next line
    if (!infile->ReadParseLine(2,funcname)) return false;
    // now that we have the parser line, we get
    if (!infile->lp.SIntVal(0,insert_index)) return infile->BuildError("The page index is not a number");
    if (!infile->lp.SIntVal(1,grand_total)) return infile->BuildError("The grand item total is not a number");
    return true;

    // extra...
    if (!ReadDelta(infile)) return false;

    // item count is already read, we assume the line index is just before the first item
    for (size_t loopc = 0; loopc < expected_item_count; loopc++) {
        jfItemFlagGroup temp = jfItemFlagGroup{NULL,NULL};
        if (ReadGroupFromFile(infile, temp)) {
            mainlist.push_back(temp);
        }
        // if the read failes, we may have to clear an item returning
        else {
            if (temp.item != NULL) {
                delete temp.item;
                temp.item = NULL;
            }
            return false;
        }
    }
    // we should be left just before a final line...
    if (!infile->ReadLast(funcname)) return false;
    return true;
    // done
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfSearchResultItemCollectionBase::WriteGroupToFile(const jfItemFlagGroup& group, QTextStream* outfile) const {
    // first, the item, using the build-in method
    bool item_okay = group.item->AddToFile(outfile);
    if (!item_okay) return false;
    // now, the flags
    if (group.flags == NULL) (*outfile) << "F\n";
    else {
        jfOutString flag_out;
        flag_out <<"F" << ((size_t)(group.flags->category).to_ulong());
        flag_out << UpdateStatusToString(group.flags->ustatus);
        flag_out << (group.flags)->included << (group.flags)->ignore;

        (*outfile) << flag_out << "\n";
    }
    return true;
}
// --------------------------------------------------------
bool jfSearchResultItemCollectionBase::ReadGroupFromFile(jfFileReader* infile, jfItemFlagGroup& target_group) const {
    const QString fname = "jfSearchResultItemCollectionBase::ReadGroupFromFile";
    if (ReadItemFromFile(infile, target_group)) {
        QString rawline;
        if (!infile->ReadLine(rawline, fname)) return false;
        if (rawline.trimmed() == "F") return true;
        else {
            jfLineParse lp = jfLineParse(rawline);
            if (lp.NNotX(5)) return infile->BuildError("Flags line does not have 5 fields!");
            // category bitset
            quint32 raw_categories = 0;
            if (!lp.UIntVal(1,raw_categories)) {
                return infile->BuildError("Categories could not be converted to an integer!");
            }
            // update status
            jfUpdateStatus ustatus = StringToUpdateStatus(lp.Get(2));
            // flags
            bool included;
            if (!lp.BoolVal(3,included)) {
                return infile->BuildError("The included flag could not be parsed!");
            }
            bool ignore;
            if (!lp.BoolVal(4,ignore)) {
                return infile->BuildError("The ignore flag could not be parsed!");
            }
            // making flags
            target_group.flags = new jfItemMetaFlags();
            target_group.flags->category = std::bitset<32>(raw_categories);
            target_group.flags->ustatus = ustatus;
            target_group.flags->included = included;
            target_group.flags->ignore = ignore;
            if (ignore) {
                target_group.flags->background_color = defcolour::ignored;
            }

            // done
            return true;
        }
    }
    else return false;
}



/*
protected:
// page (insertion) tracking
size_t insert_index;
size_t grand_total;
// the collection
std::vector<jfItemFlagGroup> mainlist;
// id tracking for duplicate filtering
jfIDStore* idstore;
*/


// ===========================================================================
/*
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
*/
//----------------------------------------------------------------------
/* Checks the id's of the objects in new_items against the idstore. If
 * already included, the objects are marked 'included = false' (since they
 * should not be added). Otherwise, the id's are added to the store. */
/*
size_t jfUrlItemCollection::IDFilterMarkResults(jfResultUnitVector* new_items) {
  // trivial wrong cases
  if (idstore == NULL) return 0;
  if (new_items == NULL) return 0;
  if (new_items->size() == 0) return 0;
  // We loop thru the items, checking against the list (we assume no duplicates *within* the input items.
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
*/
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented virtual i/o methods
//----------------------------------------------------------------------------
/* What we write here is page index, page count, and grand total. I've decided
*not* to include search core, old index, or intermediate info here */
/*
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
*/
/*****************************************************************************/
