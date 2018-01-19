/******************************************************************************
Name    :   fimgroup_thread.cpp
Author  :   John Q Metro
Purpose :   FIM Groups search thread
Created :   August 4, 2015
Updated :   January 18, 2018
******************************************************************************/
#ifndef FIMGROUP_THREAD_H
  #include "fimgroup_thread.h"
#endif // FIMGROUP_THREAD_H
//----------------------------------------------
#ifndef FIM_GROUPSEARCH_H_INCLUDED
  #include "../../data/fim/fim_groupsearch.h"
#endif // FIM_GROUPSEARCH_H_INCLUDED

#ifndef NEWFIMGROUP_PARSER_H
    #include "newfimgroup_parser.h"
#endif // NEWFIMGROUP_PARSER_H
#ifndef FIMGROUP_PARSER_H
  #include "fimgroup_parser.h"
#endif // FIMGROUP_PARSER_H
#ifndef HTMLPARSE_H_INCLUDED
  #include "../../../core/utils/htmlparse.h"
#endif // HTMLPARSE_H_INCLUDED
//-----------------------------------------
#include <assert.h>
/*****************************************************************************/
// the constructor
jfFIMGroup_DownThread::jfFIMGroup_DownThread(size_t in_max_threads):jfBaseItemDownloader(in_max_threads) {
  maindata = NULL;
  current_collection = NULL;
  search_object = NULL;
  skip_on_fail = false;
  card_phase = true;
  do_card = false;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++
void jfFIMGroup_DownThread::StartProcessing() {
    const QString fname = "jfFIMGroup_DownThread::StartProcessing";
    initLog("FIMGroup");
    // initial prep...
    emit SendStart(true);
    /**/tLog(fname,1);
    started = true;
    search = dynamic_cast<jfFIMGroupSearch*>(search_object);
    current_collection = maindata->GetCurrPointer();
    do_card = search->NoFilters();
    /**/tLogB(fname,2,do_card);
    // doing 'card' search first (36 groups per page, no descriptions)
    if (do_card) search->DispatchCategory();
    bool result = FetchAllPages(2000);
    /**/tLogB(fname,3,result);
    // handling the results of the card search
    if (!result) {
        AllDone(false);
        return;
    }
    else if (do_card) {
        AllDone(true);
        return;
    }
    // switching to individual group download
    /**/tLog(fname,4);
    card_phase = false;
    emit SendReset();
    size_t est_pcount = card_store.size();
    /**/tLogS(fname,5,est_pcount);
    // doing full search
    search_object->DispatchCategory();
    result = FetchAllPages(est_pcount);
    /**/tLogB(fname,6,result);
    // done
    DeleteCardStore();
    /**/tLog(fname,7);
    AllDone(result);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++
// special methods
size_t jfFIMGroup_DownThread::StoreCardResults(jfResultUnitVector* in_vector) {
    // inputs that have nothing
    if (in_vector == NULL) return 0;
    if ((in_vector->size()) == 0) return 0;
    // preparation for the loop
    jfResultUnitVector* one_item_vector = NULL;
    // each input item is stored in a new vector, which is stored in the card_store (a vector)
    for (size_t inv_index = 0 ; inv_index < (in_vector->size()) ; inv_index++) {
        one_item_vector = new jfResultUnitVector();
        one_item_vector->push_back((*in_vector)[inv_index]);
        (*in_vector)[inv_index] = NULL;
        card_store.push_back(one_item_vector);
    }
    size_t result = in_vector->size();
    in_vector->clear(); // the calling method should delete in_vector afterwards
    return result;
}
//-------------------------------------------------
void jfFIMGroup_DownThread::DeleteCardStore() {
    // current temp pointers
    jfResultUnit* cunit;
    jfBasePD* item;
    // the main 'going over the vector' loop
    for (size_t cardDex = 0; cardDex < card_store.size(); cardDex++) {
        // getting the result unit
        cunit = (*card_store[cardDex])[0];
        // deleting the attached result
        item = const_cast<jfBasePD*>(cunit->Sourcelink());
        delete item;
        // deleting the result unit
        delete cunit;
        // clearing the one item vector
        (*card_store[cardDex])[0] = NULL;
        card_store[cardDex]->clear();
        delete card_store[cardDex];
        card_store[cardDex] = NULL;
    }
    // with this loop over, card_store should have NULL in all positions, we clear
    card_store.clear();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++
jfItemsPageParserBase* jfFIMGroup_DownThread::makeItemParser() {
    if (card_phase) return new jfNewFIMGroupParser();
    else return new jfFIMGroupParser();
}
//------------------------------------------------
void jfFIMGroup_DownThread::makeFirstPageInfo() {
  PrepareItemInfo(1);
}
//------------------------------------------------
void jfFIMGroup_DownThread::ProcessFirstPageResults(void* resultdata) {
    assert(resultdata!=NULL);
    ProcessResults(resultdata);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// overriden methods
bool jfFIMGroup_DownThread::ProcessResults(void* resultdata) {
    // here, we split up the results and store them for phase 2
    if ((card_phase) && (!do_card)) {
        jfResultUnitVector* res_vector = static_cast<jfResultUnitVector*>(resultdata);
        StoreCardResults(res_vector);
        delete res_vector;
        return true;
    }
    // process and display the results as per normal...
    else {
        return jfBaseItemDownloader::ProcessResults(resultdata);
    }
}
//-----------------------------------
void jfFIMGroup_DownThread::DisposeResults(void* resultdata) {
    jfBaseItemDownloader::DisposeResults(resultdata);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implemented virtual methods
//----------------------------
QString* jfFIMGroup_DownThread::makeURLforPage(size_t index) const {
    const QString fname = "jfFIMGroup_DownThread::makeURLforPage";
    if (card_phase) return new QString(search->GetIndexUrl(index));
    else {
        /**/tLogS(fname,1,index);
        const jfResultUnit* current_result = (*card_store[index-1])[0];
        return new QString(current_result->GetUrl());
    }
}
//----------------------------
void jfFIMGroup_DownThread::PrepareItemInfo(size_t pageIndex) {
    // setting up the label values
    infoToSend.startaction = "Doing";
    infoToSend.part_count = pagecount;
    infoToSend.item_index = pageIndex;
    if (card_phase) {
        infoToSend.item_name = "";
        infoToSend.item_label = "Card Page";
    }
    else {
        const jfResultUnit* current_result = (*card_store[pageIndex-1])[0];
        QString disp_name = htmlparse::ConvertEntities(current_result->GetName(),false);
        infoToSend.item_name = disp_name;
        infoToSend.item_label = "Group";
  }
}
/*****************************************************************************/
