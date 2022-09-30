/***************************************************************
 * Name:      ficdown_tbase.cpp
 * Purpose:   Base downloader thread for fanfic downloading (and writing)
 * Author:    John Q Metro
 * Created:   July 10, 2016
 * Updated:   July 7, 2022
 *
 **************************************************************/
#ifndef FICDOWN_TBASE_H
  #include "ficdown_tbase.h"
#endif // FICDOWN_TBASE_H
//-------------------------------
#ifndef MAKEPARSER_H
  #include "../parsers/makeparser.h"
#endif // MAKEPARSER_H
#include <assert.h>

/**************************************************************/
jfFicDownloaderBase::jfFicDownloaderBase():jfDownloadRootFirstPage() {
  fic_data = NULL;
  fanfic = NULL;
  numfiles = 0;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++
// virtual methods that are implemented here
//-----------------------------------------
QString* jfFicDownloaderBase::makeURLforPage(size_t index) const {
  assert(index>0);
  if (index==1){
    if (fic_data !=NULL) return new QString(fic_data->FirstUrl());
    return new QString(firsturl);
  }
  else {
    if (indexpage) return new QString(fic_data->MakeUrl(index-1));
    else return new QString(fic_data->MakeUrl(index));
  }
}
//-----------------------------------------
bool jfFicDownloaderBase::ProcessResults(void* resultdata) {
  // this method should only be used on later pages.
  assert(result_page_index > 0);
  jfFicPart* ficpart = static_cast<jfFicPart*>(resultdata);
  fanfic->AddPart(ficpart);
  return true;
}
//-----------------------------------------
void jfFicDownloaderBase::DisposeResults(void* resultdata) {
  // the first page should never need to be disposed
  assert(result_page_index > 0);
  jfFicPart* throwaway = static_cast<jfFicPart*>(resultdata);
  delete throwaway;
}
//-----------------------------------------
void jfFicDownloaderBase::PrepareItemInfo(size_t pageIndex) {
  assert(pageIndex>1);
  infoToSend.startaction = "Doing";
  infoToSend.item_label = "Part";
  if (indexpage) infoToSend.item_index = pageIndex -1;
  else infoToSend.item_index = pageIndex;
  infoToSend.part_count = fic_data->pcount;
  infoToSend.item_name = fic_data->NameForPart(infoToSend.item_index);
}
//-----------------------------------------
jfPageParserBase* jfFicDownloaderBase::makeParser() {
  return MakeTypedFicParser(fictype);
}
//------------------------------
jfParseFetchPackage* jfFicDownloaderBase::MakeParserFetcher() {
    jfPageParserBase* parser = makeParser();
    if (parser == NULL) return NULL;
    return DefaultParseFetchMaker(fictype, jglobal::FPT_FICPART_PAGE, parser);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++
// first page  methods
//-----------------------------------------
void jfFicDownloaderBase::makeFirstPageInfo() {
  infoToSend.startaction = "Doing";
  if (indexpage) {
    infoToSend.item_index = -1;
    infoToSend.item_label = "Index Page";
    infoToSend.part_count = -1;
  }
  else {
    infoToSend.item_index = 1;
    infoToSend.item_label = "Part";
    if (fic_data == NULL) infoToSend.part_count = -1;
    else {
      infoToSend.part_count = fic_data->pcount;
      infoToSend.item_name = fic_data->NameForPart(1);
    }
  }
  infoToSend.item_name = "";
}
//-----------------------------------------
void jfFicDownloaderBase::ProcessFirstPageResults(void* resultdata) {
  const QString fname = "jfFicDownloaderBase::ProcessFirstPageResults";
  assert(resultdata!=NULL);
  // variables
  jfFicExtract* extracted_data;
  jfFicPair* combo_results;
  // putting a type to the results
  /**/tLog(fname,1);
  if (indexpage) extracted_data = static_cast<jfFicExtract*>(resultdata);
  else {
    combo_results = static_cast<jfFicPair*>(resultdata);
    extracted_data = combo_results->extract;
  }
  /**/tLog(fname,2);
  // testing if we abort
  abort_fetch = false;
  if (TestForAbort(extracted_data)) {
    /**/tLog(fname,3);
    abort_fetch = true;
    delete extracted_data;
    if (!indexpage) {
      /**/tLog(fname,4);
      delete (combo_results->part_one);
      delete combo_results;
    }
    return;
  }
  /**/tLog(fname,5);
  // otherwise, we handle the results...
  // handling the extract...
  jfFicExtract* fcopy = CopyExtract(extracted_data);
  fic_data = extracted_data;
  emit sendFicExtract(fcopy);
  /**/tLog(fname,6);
  // setting up the fic
  fanfic = new jfFanfic(fictype);
  fanfic->SetFic(fic_data,false);
  SetFileBase();
  // adding the first part, if we have it
  if (!indexpage) {
    /**/tLog(fname,7);
    fanfic->AddPart(combo_results->part_one);
    delete combo_results;
  }
  // that's all
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfFicDownloaderBase::SendFileMsg(const size_t& fileindex) {
  infoToSend.startaction = "Writing";
  infoToSend.item_label = "File";
  infoToSend.item_index = fileindex;
  infoToSend.part_count = numfiles;
  infoToSend.item_name = "";
  emit SendNewItem(infoToSend);
}
//----------------------------------------------
void jfFicDownloaderBase::ClearData() {
  delete fanfic;
  fanfic = NULL;
  fic_data = NULL;
  indexpage= false;
}
//----------------------------------------------
// implemented fic download *and writing it to a file!*
/* For this, fictype must be correct, and either fic_data OR
 * firsturl must be set. */
bool jfFicDownloaderBase::xDownloadFic() {
  const QString fname = "jfFicDownloaderBase::DownloadFic";
  /**/tLog(fname,1);
  if (fic_data != NULL) pagecount = fic_data->pcount;
  else pagecount = 10; // pure guess!
  emit SendItemCount(pagecount);
  /**/tLogS(fname,2,pagecount);
  // index page
  indexpage = SiteHasIndexpage(fictype);
  // fetching
  bool result = xFetchAllPages(pagecount);
  /**/tLogB(fname,3,result);
  /**/tLogB(fname,4,abort_fetch);
  if (abort_fetch) return result;
  if (!result) return false;
  /**/tLog(fname,6);
  // if the fic download has not been aborted or failed, we write
  emit SendReset();

  // initial prep
  emit SendItemCount((fic_data->pcount) + 1);
  /**/tLog(fname,7);
  infoToSend.startaction = "Calculating";
  infoToSend.item_label = "Fanfic Split ...";
  infoToSend.item_index = -1;
  infoToSend.part_count = -1;
  infoToSend.item_name = "";
  emit SendNewItem(infoToSend);
  result = fanfic->StartResult();
   /**/tLogB(fname,8,result);
  numfiles = fanfic->GetFilecount();
  emit SendItemDone();
  emit SendItemCount(1+numfiles);
  /**/tLog(fname,9);

  // writing the individual parts
  for (size_t findex = 1 ; findex <= numfiles ; findex++) {
    /**/tLogL(fname,10,findex);
    SendFileMsg(findex);
    fanfic->WriteFile(findex);
    emit SendItemDone();
    /**/tLog(fname,11);
  }

  // cleanup
  fanfic->FinishAfter();
  ClearData();
  return true;
}


/**************************************************************/
