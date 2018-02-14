/*****************************************************************************
Name    : fetchandparse.h
Basic   : Basal threads for download and parsing
Author  : John Q Metro
Started : October 17, 2015
Updated : June 26, 2016

******************************************************************************/
#ifndef FETCHANDPARSE_H
  #include "fetchandparse.h"
#endif // FETCHANDPARSE_H
//-------------------------------
#include <assert.h>
#include <math.h>
#include <QCoreApplication>
/*****************************************************************************/
// --- [ METHODS for jfFetchAndParsePage ] ---
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// setup and setting data
//----------------------------------------
jfFetchAndParsePage::jfFetchAndParsePage():jfLoggBase() {
  parser = NULL;
  pauser = NULL;
  channel = NULL;
  started = false;
  downloader = NULL;
  fetched_page = NULL;
  urlIndex = 0;
}
//----------------------------------------
bool jfFetchAndParsePage::setChannel(jfThreadDataPasser* in_channel) {
  if (started) return false;
  if (in_channel == NULL) return false;
  channel = in_channel;
  return true;
}
//----------------------------------------
bool jfFetchAndParsePage::setParser(jfPageParserBase* in_parser) {
  if (started) return false;
  if (in_parser == NULL) return false;
  parser = in_parser;
  parser->SetLogPointer(this);
  return true;
}
//----------------------------------------
bool jfFetchAndParsePage::setPauser(jfPauseStop* input_pauser){
  if (started) return false;
  if (input_pauser == NULL) return false;
  pauser = input_pauser;
  return true;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// destructor
jfFetchAndParsePage::~jfFetchAndParsePage() {
  if (parser!=NULL) {
    delete parser;
    parser = NULL;
  }
  if (downloader!=NULL) {
    delete downloader;
  }
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void jfFetchAndParsePage::StartProcessing() {
  const QString fname = "jfFetchAndParsePage::StartProcessing";
  // temp variable
  const QString* ures = NULL;
  jfFETCH_ERROR currError = jff_NOERROR;
  jfDownloadResults *resultStore;
  QString parse_err;
  void* res_data;
  size_t new_pagecount;
  // initial do once setup
  started = true;
  initLog("FetchAndParse");
  /**/tLog(fname,1);
  downloader = new jfFetchPage();
  downloader->SetParams(parser);
  /**/tLog(fname,2);
  // the main loop, continues until get get a NULL URL
  while (fname,true) {
    /**/tLog(fname,3);
    // getting the URL (may block)
    ures = channel->GetUrl(urlIndex);
    /**/tLogB(fname,4,ures == NULL);
    // checking URL afterwards
    if (ures==NULL) break;
    urlToGet = *ures;
    /**/tLog(fname,5,urlToGet);
    currError = DownloadMethod();
    if (currError==jff_NOERROR) {
      /**/tLog(fname,6);
      // time to parse!
      parser->ParseDownloadedPage((*fetched_page),urlIndex);
      delete fetched_page;
      fetched_page = NULL;
      // parsing went okay
      if (parser->isPageParsed()) {
        /**/tLog(fname,7);
        res_data = parser->getResults();
        new_pagecount = parser->getPageCount();
        /**/tLogS(fname,8,new_pagecount);
        channel->ResultResultData(res_data,new_pagecount);
        /**/tLog(fname,9);
      }
      // parsing did not go okay
      else {
        parse_err = parser->getParseErrorMessage();
        /**/tLog(fname,10,parse_err);
        resultStore = new jfDownloadResults();
        (resultStore->fetchResults).why_more = parse_err;
        (resultStore->fetchResults).why = jfa_PARSEERR;
        (resultStore->fetchResults).halt = true;
        /**/tLog(fname,11);
        channel->ReturnResults(resultStore);
        /**/tLog(fname,12);
      }
      /**/tLog(fname,13);
    }
    else {
      // sending back an error message instead
      /**/tLog(fname,14);
      // redirection
      if (currError == jff_REDIRECTION) {
        /**/tLog(fname,15);
        resultStore = new jfDownloadResults(redirectionResult);
      }
      // some other problem
      else {
        /**/tLog(fname,16,jf_FetchErr2String(currError,false));
        resultStore = new jfDownloadResults();
        (resultStore->fetchResults).why = jf_Error2AfterMath(currError);
        /**/tLog(fname,17);
        (resultStore->fetchResults).halt = true;
      }
      /**/tLog(fname,18);
      // passing back the result
      channel->ReturnResults(resultStore);
      /**/tLog(fname,19);
    }
    // going on to the next item...
    /**/tLog(fname,20);
  }
  // we are done with this thread
  /**/tLog(fname,21);
  started = false;
  delete downloader;
  downloader = NULL;
  CloseLog();
  /**/tLog(fname,22);
  emit processingFinished();
}
//++++++++++++++++++++++++++++++++++++++++++++++
// bundles a few downloading related lines
bool jfFetchAndParsePage::DownloadPage() {
    const QString fname = "jfFetchAndParsePage::DownloadPage";
    /**/tLog(fname,1);
  downloader->SetURL(urlToGet);
  /**/tLog(fname,2);
  downloader->StartDownload();
  /**/tLog(fname,3);
  return downloader->PrepareResults();
}
//--------------------------------------------
/* The downloader, with redirection and retry loops, urlToGet must be set */
jfFETCH_ERROR jfFetchAndParsePage::DownloadMethod() {
  const QString fname = "jfFetchAndParsePage::DownloadMethod";
  // variables
  size_t retry_count = 0;
  bool download_okay = false;
  QString *ures;
  jfFETCH_ERROR currError = jff_NOERROR;
  /**/tLog(fname,1);
  // download
  while (retry_count < 5) {
      /**/tLogS(fname,2,retry_count);
    download_okay = DownloadPage();
    // if the download is fine, we get the page and exit the loop
    if (download_okay) {
        /**/tLog(fname,3);
      fetched_page = downloader->GetResult();
      downloader->Reset();
      break;
    }
    // handling a non-okay attempt at downloading
    else {
        /**/tLog(fname,4);
      currError = downloader->GetError();
      // redirection is the cause...
      if (currError == jff_REDIRECTION) {
          /**/tLog(fname,5);
        redirectionResult = downloader->GetRedirectTarget();
        ures = parser->makeRedirectedURL(redirectionResult);
        downloader->Reset();
        if (ures == NULL) break;
        else {
          urlToGet = *ures;
          delete ures;
        }
      }
      // hard download error, trying gain means nothing
      else if (currError != jff_TRYAGAIN){
          /**/tLog(fname,6);
        downloader->Reset();
        break;
      }
      // here, if we try again, it might work!
      else downloader->Reset();
    }
    /**/tLog(fname,7);
    // if we get here, we are retrying for the next loop
    retry_count++;
    jfSleepThread::msleep(1000);
    QCoreApplication::processEvents();
  }
  /**/tLogB(fname,8,download_okay);
  if (download_okay) return jff_NOERROR;
  else return currError;
}

//============================================================================================
// constructor and destructor
//---------------------------------
jfFetchAndParseHolder::jfFetchAndParseHolder() {
  currentURL = NULL;
  thread_active = false;
  is_fetching = false;
  thread_finished = false;
  // creating the objects
  thread_ptr = new QThread();
  thread_object = new jfFetchAndParsePage();
  channel = new jfThreadDataPasser();
}
//---------------------------------
void jfFetchAndParseHolder::SetupLinks() {
  // linking the objects
  thread_object->setChannel(channel);
  thread_object->moveToThread(thread_ptr);
  // thread management signals and slots
  connect(thread_ptr, SIGNAL(started()), thread_object, SLOT(StartProcessing()));
  connect(thread_object,SIGNAL(processingFinished()),thread_ptr,SLOT(quit()));
  connect(thread_ptr,SIGNAL(finished()),this,SLOT(ThreadFinished()));
}

//---------------------------------
jfFetchAndParseHolder::~jfFetchAndParseHolder() {
  if (thread_active) {
    // killing the thread, this is bad (so best to delete only after ThreadFinished()).
    disconnect(thread_ptr, SIGNAL(started()), thread_object, SLOT(StartProcessing()));
    disconnect(thread_ptr,SIGNAL(finished()),this,SLOT(ThreadFinished()));
    disconnect(thread_object,SIGNAL(processingFinished()),thread_ptr,SLOT(quit()));
    thread_ptr->terminate();
    delete channel;
    delete thread_object;
    delete thread_ptr;
  }
  if (currentURL != NULL) delete currentURL;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// info methods
//--------------------
bool jfFetchAndParseHolder::isActive() const {
  return thread_active;
}
//--------------------
bool jfFetchAndParseHolder::isFetching() const {
  return is_fetching;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// thread manipulation methods
//----------------------------------------------------------------
bool jfFetchAndParseHolder::Start() {
  if (thread_active || thread_finished) return false;
  if (channel == NULL) return false;
  thread_active = true;
  thread_ptr->start();
  return true;
}

//-----------------------------------------------------------------
bool jfFetchAndParseHolder::SignalEnd() {
  if (!thread_active) return false;
  if (thread_finished) return false;
  if (channel==NULL) return false;
  channel->SendUrl(NULL,0);
  return true;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfFetchAndParseHolder::SetPauserAndParser(jfPauseStop* in_pauser, jfPageParserBase* in_parser) {
  if ((in_pauser==NULL) || (in_parser==NULL)) return false;
  if (thread_active || thread_finished) return false;
  thread_object->setPauser(in_pauser);
  thread_object->setParser(in_parser);
  return true;
}
//-----------------------------------------------------------------
bool jfFetchAndParseHolder::SendURL(QString* url_to_send, const size_t& fetch_page_index) {
  if (!thread_active) return false;
  if (channel == NULL) return false;
  if (currentURL != NULL) delete currentURL;
  currentURL = url_to_send;
  is_fetching = true;
  channel->SendUrl(url_to_send,fetch_page_index);
  return true;
}
//-----------------------------------------------------------------
jfDownloadResults* jfFetchAndParseHolder::GetResults() {
  const QString fname = "jfFetchAndParseHolder::GetResults";
  if (!thread_active) return NULL;
  if (thread_finished) return NULL;
  if (channel==NULL) return NULL ;
  jfDownloadResults* temp = channel->GetResults();
  is_fetching = false;
  return temp;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// slot called when thread is done
void jfFetchAndParseHolder::ThreadFinished() {
  const QString fname = "jfFetchAndParseHolder::ThreadFinished";
  thread_active = false;
  /**/JDEBUGLOG(fname,1);
  // disconnecting signals from slots
  disconnect(thread_ptr, SIGNAL(started()), thread_object, SLOT(StartProcessing()));
  disconnect(thread_ptr,SIGNAL(finished()),this,SLOT(ThreadFinished()));
  disconnect(thread_object,SIGNAL(processingFinished()),thread_ptr,SLOT(quit()));
  // deleting the contained objects
  thread_finished = true;
  channel->deleteLater();
  thread_object->deleteLater();
  thread_ptr->deleteLater();
  channel = NULL;
  thread_object = NULL;
  thread_ptr = NULL;
  /**/JDEBUGLOG(fname,2);
  // done
  emit ThreadDone();
}

//************************************************************************************************************
