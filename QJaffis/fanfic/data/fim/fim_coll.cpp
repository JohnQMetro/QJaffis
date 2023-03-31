/******************************************************************************
Name    :   fim_coll.h
Author  :   John Q Metro
Purpose :   Collections for fimfiction.net data
Created :   May 22, 2012
Conversion to Qt started November 9, 2013
Updated :   March 19, 2023
******************************************************************************/
#ifndef FIM_COLL_H_INCLUDED
  #include "fim_coll.h"
#endif // FIM_COLL_H_INCLUDED
#ifndef FIM_SEARCH_H_INCLUDED
  #include "fim_search.h"
#endif // FIM_SEARCH_H_INCLUDED
#ifndef UTILS3_H_INCLUDED
  #include "../../../core/utils/utils3.h"
#endif // UTILS3_H_INCLUDED
#ifndef SKELCOLL_H_INCLUDED
  #include "../../../core/structs/skelcoll.h"
#endif // SKELCOLL_H_INCLUDED
//---------------------------------------------------------------
//*****************************************************************************
// alternate constructor for use when loading data from file
jfFIMItemCollection::jfFIMItemCollection(QString&& in_name, size_t in_num_id):jfSearchResultsCollection<jfFIM_Fanfic>(in_name, in_num_id) {

}
jfFIMItemCollection::jfFIMItemCollection(const QString& in_name, size_t in_num_id):jfSearchResultsCollection<jfFIM_Fanfic>(in_name, in_num_id) {

}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented page methods
//-----------------------------------------------------
QString jfFIMItemCollection::TypeId() const {
  return "FIMItemCollection";
}
//-----------------------------------------------------
bool jfFIMItemCollection::LoadValues(jfSkeletonParser* inparser,size_t which) const {
  LoadCoreValues(inparser,which);
  return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// getting more information
jfFicExtract_FIM* jfFIMItemCollection::FicExt_AtIndex(size_t i_index) const {
  if (i_index >= mainlist.size()) return NULL;
  else {
      const jfFIM_Fanfic* fanfic = dynamic_cast<const jfFIM_Fanfic*>(mainlist.at(i_index).item);
      return dynamic_cast<jfFicExtract_FIM*>(fanfic->GetExtract());
  }
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// some more virtual i/o methods
//-----------------------------------------------------
bool jfFIMItemCollection::ReadItemFromFile(jfFileReader* infile, jfItemFlagGroup& target_group) const {
    jfFIM_Fanfic* fanfic = new jfFIM_Fanfic();
    if (fanfic->GetFromFile(infile)) {
        target_group.item = fanfic;
        return true;
    }
    else {
        delete fanfic;
        return false;
    }
}
//-----------------------------------------------------
bool jfFIMItemCollection::AddDelta(QTextStream* outfile) const {
  return true;
}
//-----------------------------------------------------
bool jfFIMItemCollection::ReadDelta(jfFileReader* infile) {
  return true;
}

//=============================================================================
// constructor
jfFIMResColl::jfFIMResColl(jfFIMSearch* xsearch_in):jfResultCollection(xsearch_in, false) {
  typed_search = xsearch_in;
  jfSearchResultItemCollectionBase* xc = MakeEmptyCollection();
  AddItem(xc);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
QString jfFIMResColl::GetTypeID() const {
  return "FIMResColl";
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfFIMResColl::WriteToHTML(size_t result_category) {
  // local variables
  jfHtmlParams *output;
  jfFIMItemCollection* temp;
  // checks
  assert(result_category<33);
  // starting
  output = OpenHTMLOutfile(result_category);
  // we prepare the skeletop
  output->base = search->GetLocalSkeleton();
  if ((output->base)==NULL) {
    output->base = mainskelc->AtIndexS(3);
  }
  // we load up with values first
  search->LoadValues(output->parse,output->GetResCat());
  // header
  WriteHtmlHeader(output);
  // body
  temp = dynamic_cast<jfFIMItemCollection*>((*collections)[0]);
  temp->WriteToHTML(output,true);
  // finishing off
  WriteFooterClose(output);
  return true;
}
//-------------------------------------------------------------------------
jfFicExtract_FIM* jfFIMResColl::GetExtract(size_t xindex) const {
  // variables
  jfFIMItemCollection* typox;
  // starting
  (*cprotect)[0]->lock();
  typox = dynamic_cast<jfFIMItemCollection*>((*collections)[0]);
  // getting the data
  jfFicExtract_FIM* xres = typox->FicExt_AtIndex(xindex);
  (*cprotect)[0]->unlock();
  // done
  return xres;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfFIMResColl::WriteHtmlHeader(jfHtmlParams* indata) {
  // local variables
  QString buffer;
  // the usual checks
  assert(indata!=NULL);
  assert(indata->CheckOk());
  // loading values
  // constatnts
  buffer = indata->MakeResult(indata->base->header_skel);
  indata->AppendLine(buffer);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented private i/o methods
//--------------------------------------
bool jfFIMResColl::AddRestToFile(QTextStream* outfile) const {
  // does nothing
  return true;
}
//--------------------------------------
jfSearchResultItemCollectionBase* jfFIMResColl::MakeEmptyCollection() const {
  jfFIMItemCollection* result;
  result = new jfFIMItemCollection("FIM Item List",1);
  return result;
}
//--------------------------------------
bool jfFIMResColl::ReadRestFromFile(jfFileReader* infile) {
  return true;
}

//*****************************************************************************
