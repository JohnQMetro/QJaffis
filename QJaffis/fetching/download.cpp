/*****************************************************************************
Name    : download.cpp
Basic   : Download controller threads
Author  : John Q Metro
Started : June 16, 2015
Updated : September 4, 2016
******************************************************************************/
#ifndef DOWNLOAD_H
  #include "download.h"
#endif // DOWNLOAD_H
//-------------------------------
#include <assert.h>
#include <math.h>
#include <QCoreApplication>
/*****************************************************************************/

jfBaseDownloader::jfBaseDownloader(size_t in_max_threads) {
  assert(in_max_threads>0);
  max_threads = in_max_threads;
  url_page_index = 0;
  result_page_index = 0;
  actual_threads = 0;
  pagecount = 0;
  current_index = 0;
  started = false;
  skip_on_fail = false;
}
//----------------------------------------------------------
bool jfBaseDownloader::setPauser(jfPauseStop* in_pauser) {
  if (in_pauser==NULL) return false;
  if (started) return false;
  pauser = in_pauser;
  return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfBaseDownloader::handleThreadDone() {
  done_threads++;
  if (done_threads == actual_threads) all_done = true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/* The loopmax value is not used much, it is the pagecount
 * rounded up to a multiple of threadcount */
void jfBaseDownloader::CalcLoopMax() {
  if (pagecount < actual_threads) loopmax = actual_threads;
  else {
    float rounds = float(pagecount)/float(actual_threads);
    loopmax = ceil(rounds)*actual_threads;
  }
}
//-------------------------------------
/* the thread array is a circular buffer, so incrementing the
 * index into it sometimes needs to loop back to 0. */
void jfBaseDownloader::incrThreadIndex(size_t &thread_index) {
  thread_index++;
  if (thread_index == actual_threads) thread_index = 0;
}
//-------------------------------------
// methods implemented here
void jfBaseDownloader::SetupThreads(bool firstpage){
  const QString fname = "jfBaseDownloader::SetupThreads";
  jfPageParserBase* parser;
  /**/tLogB(fname,1,firstpage);
  if (firstpage) {
    assert(url_page_index == 1);
    assert(result_page_index == 1);
  }
  /**/tLog(fname,2);
  // figuring out how many threads to add
  if (firstpage) {
    if ((pagecount-1) < max_threads) actual_threads = pagecount-1;
    else actual_threads = max_threads;
  }
  else {
    if (pagecount < max_threads) actual_threads = pagecount;
    else actual_threads = max_threads;
  }
  /**/tLogS(fname,3,actual_threads);
  // calculating the ‘loopmax’ value
  CalcLoopMax();
  // adding and starting the threads
  size_t start = (firstpage)?(1):(0);
  /**/tLogS(fname,4,start);
  jfFetchAndParseHolder* tempthread;
  for ( start ; start < actual_threads ; start++) {
    /**/tLogL(fname,5,start);
    tempthread = new jfFetchAndParseHolder();
    parser = makeParser();
    tempthread->SetPauserAndParser(pauser,parser);
    tempthread->SetupLinks();
    threads.push_back(tempthread);
    connect(threads[start],SIGNAL(ThreadDone()),this,SLOT(handleThreadDone()));
    /**/tLog(fname,6);
    threads[start]->Start();
  }
  // done!
  all_done = false;
  /**/tLog(fname,7);
  done_threads = 0;
}
//----------------------------------------------
bool jfBaseDownloader::HandleResults(jfDownloadResults* in_results) {
  const QString fname = "jfBaseDownloader::HandleResults";
    assert(in_results != NULL);
    current_error = (in_results->fetchResults).why;
    /**/tLog(fname,1,jf_Aftermath2String(current_error));
    // case 1 : okay result
    if (current_error == jfa_DONE) {
        /**/tLog(fname,2);
        void* tdata = in_results->resultData;
        bool xres = ProcessResults(tdata);
        /**/tLogB(fname,3,xres);
        assert(xres);
        emit SendItemDone();
        /**/tLog(fname,4);
        return true;
    }
    // case 2 : slip
    else if (current_error == jfa_SKIP) {
      /**/tLog(fname,5);
      (in_results->fetchResults).halt = false;
      emit SendError(in_results->fetchResults);
      /**/tLog(fname,6);
      return true;
    }
    // otherwise, case 3: fail!
    else {
      /**/tLog(fname,7);
      (in_results->fetchResults).halt = !skip_on_fail;
      if (in_results->wasRedirected) {
        /**/tLog(fname,8);
        (in_results->fetchResults).why_more = "Redirected to : " + (in_results->newURL);
      }
      /**/tLog(fname,9);
      emit SendError(in_results->fetchResults);
      return false;
    }
}
//----------------------------------------------
bool jfBaseDownloader::handlePagecount(size_t pagecount_in, size_t cindex) {
  const QString fname = "jfBaseDownloader::handlePagecount";
  QString* url_to_send = NULL;

  // the pagecount has not changed...
  /**/tLogS(fname,1,pagecount);
  /**/tLogS(fname,2,pagecount_in);
  if (pagecount_in == pagecount) return true;
  // a pagecount of 0 is a signal that parsing cannot alter the pagecount
  if (pagecount_in == 0) return true;
  // pagecount has increased...
  else if (pagecount_in > pagecount) {
    /**/tLog(fname,3);
    // there could be a complication here, since we fetch ahead
    int empty_threads = int(result_page_index + actual_threads) - int(pagecount)-1;
    /**/tLogI(fname,4,empty_threads);
    if (empty_threads > 0) {
      // we need to insert the ahead fetches
      size_t pagediff = pagecount_in - pagecount;
      /**/tLogS(fname,5,pagediff);
      size_t add_amount = (pagediff < empty_threads)?(pagediff):(empty_threads);
      /**/tLogS(fname,6,add_amount);
      /* Starting from the next thread, we tell add_amount threads to fetch
       * pages starting after the last fetching page (url_page_index)    */
      size_t add_index = cindex;
      size_t added = 0;
      while(added < add_amount) {
        /**/tLogS(fname,7,added);
        incrThreadIndex(add_index);
        if (threads[add_index]->isFetching()) continue;
        url_page_index++;
        /**/tLogS(fname,8,url_page_index);
        url_to_send = makeURLforPage(url_page_index);
        /**/tLog(fname,9,*url_to_send);
        threads[add_index]->SendURL(url_to_send,url_page_index);
        /**/tLog(fname,10);
        added++;
      }
    }
    // whether we insert ahead fetches or not, we need to notify the change
    pagecount = pagecount_in;
    /**/tLog(fname,11);
    CalcLoopMax();
    /**/tLogS(fname,12,loopmax);
    emit SendItemCount(pagecount);
    // done
    return true;
  }
  // pagecount has decreased
  else {
    /**/tLog(fname,13);
    /* Not really a problem unless the new pagecount is less than the
     * page we have just finished parsing. I'm going to assume this
     * must be in the page data, so ProcessResults() will handle it. */
    if (pagecount_in < result_page_index) pagecount_in = result_page_index;
    pagecount = pagecount_in;
    CalcLoopMax();
    /**/tLog(fname,14);
    emit SendItemCount(pagecount);
    return (pagecount != result_page_index);
  }
}
//----------------------------------------------
void jfBaseDownloader::SendHaltMessage() {
  jfFetchErrorBundle out_error;
  out_error.halt = true;
  out_error.why = jfa_HALT;
  out_error.why_more = "User requested halt.";
  emit SendError(out_error);
}
//----------------------------------------------
void jfBaseDownloader::EndWaitAndClean() {
  const QString fname = "jfBaseDownloader::EndWaitAndClean";
  size_t limitcounter = 0;

  // waiting until all threads are finished
  /**/tLog(fname,1);
  while ((!all_done) && (limitcounter < 200)) {
    jfSleepThread::msleep(200);
    QCoreApplication::processEvents();
    /**/tLogS(fname,2,limitcounter);
    /**/tLog2S(fname,3,done_threads,actual_threads);
    limitcounter++;
  }
  /**/tLog(fname,4);
  // cleaning up when all done
  if (all_done) {
    /**/tLog(fname,5);
    for (size_t tindex = 0 ; tindex < actual_threads ; tindex++) {
      delete threads[tindex];
      /**/tLogS(fname,6,tindex);
      threads[tindex] = NULL;
    }
    /**/tLog(fname,7);
    threads.clear();
    actual_threads = 0;
    loopmax = 0;
    /**/tLog(fname,8);
  }
  // else
  else assert(false);
}

//----------------------------------------------
bool jfBaseDownloader::LoopGet(){
  const QString fname = "jfBaseDownloader::LoopGet";
  // variables
  QString* url_to_send;
  jfDownloadResults* current_results;
  bool haltmode = false;
  size_t tpagecount;
  bool pcountr;
  bool halting;

  // initially, we only pass URL's
  /**/tLog(fname,1);
  for (current_index = 0 ; current_index < actual_threads ; current_index++){
    /**/tLogL(fname,2,current_index);
    if (advanceFetchIndex()) {
      url_to_send = makeURLforPage(url_page_index);
      /**/tLog(fname,3,*url_to_send);
      threads[current_index]->SendURL(url_to_send,url_page_index);
    }
  }
  // the loop after that is handling results
  current_index = 0;
  /**/tLog(fname,5);
  while(result_page_index < pagecount) {
    // preparing to get the results (and we do send a message)
    if (!advanceResultIndex()) break;
    /**/tLogL(fname,7,result_page_index);
    PrepareItemInfo(result_page_index);
    /**/tLog(fname,8);
    emit SendNewItem(infoToSend);
    /**/tLogS(fname,9,result_page_index);
    // getting and handling the results
    current_results = threads[current_index]->GetResults();
    /**/tLogS(fname,10,current_index);
    tpagecount = current_results->pagecount;
    /**/tLogS(fname,11,tpagecount);
    haltmode = (!HandleResults(current_results)) && (!skip_on_fail);
    delete current_results;
    current_results = NULL;
    pcountr = handlePagecount(tpagecount,current_index);
    /**/tLogB(fname,12,pcountr);
    // if we halt, we break out of the loop
    if (haltmode || (!pcountr)) break;
    /**/tLogB(fname,13,pauser == NULL);
    // here, checking the pause/stop
    halting = pauser->CheckPauseStop();
    /**/tLog(fname,14);
    if (halting) {
      /**/tLog(fname,15);
      haltmode = true;
      current_error = jfa_HALT;
      SendHaltMessage();
      /**/tLog(fname,16);
      break;
    }
    /**/tLog(fname,17);
    // otherwise, we do the next url (unless there is none)
    if (url_page_index < pagecount) {
      /**/tLogS(fname,18,url_page_index);
      if (advanceFetchIndex()) {
        url_to_send = makeURLforPage(url_page_index);
        /**/tLog(fname,19,*url_to_send);
        threads[current_index]->SendURL(url_to_send,url_page_index);
      }
    }
    /**/tLogS(fname,20,current_index);
    // next index
    incrThreadIndex(current_index);
    /**/tLog(fname,21);
  }
  /**/tLog(fname,22);
  /* After the loop, stopping the threads (any maybe discarding results,
   * if there is a premature halt). */
  for (size_t xindex = 0; xindex < actual_threads ; xindex ++) {
    /**/tLogL(fname,23,xindex);
    // getting and disposing of any results
    if (threads[xindex]->isFetching()) {
      current_results = threads[xindex]->GetResults();
      if ((current_results->fetchResults).why == jfa_DONE) {
        DisposeResults(current_results->resultData);
      }
      /**/tLog(fname,24);
      delete current_results;
    }
    /**/tLog(fname,26);
    threads[xindex]->SignalEnd();
  }
  /**/tLog(fname,27);
  EndWaitAndClean();
  current_results = NULL;
  // done
  return !haltmode;
}
//------------------------------------
void jfBaseDownloader::AllDone(bool okay) {
  const QString fname = "jfBaseDownloader::AllDone";
  /**/tLog(fname,1);
  emit SendAllDone(okay);
  /**/tLog(fname,2);
  pauser->RegisterStop(current_error);
  /**/tLog(fname,3);
  started = false;
  CloseLog();
  /**/tLog(fname,4);
  emit AllFinished();
}

//============================================================================
// constructor
jfBaseFirstPageDownloader::jfBaseFirstPageDownloader(size_t in_max_threads):jfBaseDownloader(in_max_threads) {
  abort_fetch = false;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfBaseFirstPageDownloader::advanceFetchIndex() {
  url_page_index++;
  return true;
}
//-----------------------------------------------
bool jfBaseFirstPageDownloader::advanceResultIndex() {
  result_page_index++;
  return true;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// special methods
//-------------------------------
bool jfBaseFirstPageDownloader::SetupFirstThread() {
  const QString fname = "jfBaseFirstPageDownloader::SetupFirstThread";
  jfFetchAndParseHolder* firstthread = new jfFetchAndParseHolder();
  jfPageParserBase* parser = makeParser();
  /**/tLog(fname,1);
  firstthread->SetPauserAndParser(pauser,parser);
  firstthread->SetupLinks();
  threads.push_back(firstthread);
  connect(threads[0],SIGNAL(ThreadDone()),this,SLOT(handleThreadDone()));
  /**/tLog(fname,2);
  threads[0]->Start();
  /**/tLog(fname,3);
  // done!
  done_threads = 0;
  actual_threads = 1;
  all_done = false;
  /**/tLog(fname,4);
  return true;
}
//-------------------------------
bool jfBaseFirstPageDownloader::DoFirstPage() {
  size_t new_pagecount;
  const QString fname = "jfBaseFirstPageDownloader::DoFirstPage";
  // preparing for getting the data of the first page
  url_page_index = 1;
  makeFirstPageInfo();
  emit SendNewItem(infoToSend);
  /**/tLog(fname,2);
  SetupFirstThread();
  // fetching and parsing the first page
  QString* url_to_send = makeURLforPage(1);
  /**/tLog(fname,4,*url_to_send);
  threads[0]->SendURL(url_to_send,1);
  result_page_index = 1;
  // getting the results
  jfDownloadResults* page_one_results = threads[0]->GetResults();
  // handling the results
  current_error = (page_one_results->fetchResults).why;
  /**/tLog(fname,6);
  // case 1: first page was downloaded and parsed okay
  if (current_error == jfa_DONE) {
    /**/tLog(fname,7);
    // handling the results
    void* tdata = page_one_results->resultData;
    new_pagecount = page_one_results->pagecount;
    ProcessFirstPageResults(tdata);
    emit SendItemDone();
    // pagecount updating
    if ((new_pagecount != pagecount) && (new_pagecount!=0)) {
      pagecount = new_pagecount;
      CalcLoopMax();
      /**/tLog(fname,11);
      emit SendItemCount(pagecount);
    }
    /**/tLogS(fname,12,pagecount);
  }
  // case 2: the fetch/parse has failed in some way...
  else {
    (page_one_results->fetchResults).halt = !skip_on_fail;
    if (page_one_results->wasRedirected) {
      /**/tLog(fname,14,page_one_results->newURL);
      (page_one_results->fetchResults).why_more = "Redirected to : " + (page_one_results->newURL);
    }
    /**/tLog(fname,15,(page_one_results->fetchResults).why_more);
    emit SendError(page_one_results->fetchResults);
  }
  // done
  delete page_one_results;
  /**/tLog(fname,16);
  return (current_error == jfa_DONE);
}
//-------------------------------
bool jfBaseFirstPageDownloader::FetchAllPages(size_t pagecount_estimate) {
  // initial page count setup
  const QString fname = "jfBaseFirstPageDownloader::FetchAllPages";
  all_done = false;
  pagecount = pagecount_estimate;
  /**/tLogS(fname,1,pagecount);
  emit SendItemCount(pagecount_estimate);
  // handling the first page and it's results
  abort_fetch = false;
  bool page_one_result = DoFirstPage();
  /**/tLog2B(fname,2,page_one_result,abort_fetch);
  /**/tLogS(fname,3,pagecount);
  bool pagefail = ((!page_one_result) && (!skip_on_fail));
  /**/tLogB(fname,4,pagefail);
  // there are many reasons to end after the first page
  if (abort_fetch || (pagecount==1) || pagefail) {
    /**/tLog(fname,5);
    threads[0]->SignalEnd();
    /**/tLog(fname,6);
    EndWaitAndClean();
    /**/tLog(fname,7);
    return page_one_result;
  }
  // checking if the user has asked for a halt
  /**/tLog(fname,8);
  if (pauser->CheckPauseStop()) {
    current_error = jfa_HALT;
    SendHaltMessage();
    /**/tLog(fname,9);
    return false;
  }
  // if we get here, we download more pages...
  /**/tLog(fname,10);
  SetupThreads(true);
  /**/tLog(fname,10);
  bool main_results = LoopGet();
  /**/tLogB(fname,11,main_results);
  return main_results;
}

/*****************************************************************************/
