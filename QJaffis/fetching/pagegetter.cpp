/*****************************************************************************
Name    : pagegetter.cpp
Basic   : Defines my own html page fetching class (new for Qt)
Author  : John Q Metro
Started : March 16, 2013
Updated : April 18, 2018

******************************************************************************/
#ifndef PAGEGETTER_H
  #include "pagegetter.h"
#endif // PAGEGETTER_H

#ifndef LOGGING_H_INCLUDED
  #include "../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED

#ifndef CONNECT_HELPERS_H
  #include "connect_helpers.h"
#endif // CONNECT_HELPERS_H

#ifndef BASEPARSE_H
  #include "baseparse.h"
#endif // BASEPARSE_H

#include <assert.h>
//---------------------------------
#include <QByteArray>
#include <QEventLoop>
#include <QTextCodec>
#include <QSslError>
//*****************************************************************************
// --- [ PUBLIC METHODS for jfFetchPage ] ---
//++++++++++++++++++++++++++++++++++++
// constructor
jfFetchPage::jfFetchPage() {
  // initialization
  isdownloading = false;
  afterdownload = false;
  testsDelegate = NULL;
  req = NULL;
  intermediary = NULL;
  raw_result = NULL;
  thepage = NULL;
  theerror = jff_NOERROR;
  check_clientred = false;
  for (int xi=0;xi<7;xi++) errlog[xi] = 0;
}
//++++++++++++++++++++++++++++++++++++
// Setting inputs
//--------------------------
// Setting the url. Returns true if we are before a download and the url is okay
bool jfFetchPage::SetURL(const QString& inurl, const size_t& url_index) {
  // setting paramaters is only allowed before the download
  if (isdownloading) return false;
  if (afterdownload) return false;
  // setting
  fetch_this.setEncodedUrl(inurl.toLatin1());
  testsDelegate->setPageIndex(url_index);
  return fetch_this.isValid();
}
//--------------------------
// Setting non-url paramaters. Note that completely empty input are okay
bool jfFetchPage::SetParams(jfPageParserBase* in_testsDelegate) {
  // setting paramaters is only allowed before the download
  // setting the values
  testsDelegate = in_testsDelegate;
  // cookie = testsDelegate->getCookie();
  // done
  return true;
}
//++++++++++++++++++++++++++++++++++++
// main functional methods
//--------------------------
// starts the download, true if the url is valid and no download has been started
bool jfFetchPage::StartDownload() {
  // variables
  const QString fname = "jfFetchPage::StartDownload";
  QByteArray cookie_value;
  QList<QSslError> sslErrors2Ignore;
  /**/JDEBUGLOG(fname,1);
  QString cookie = testsDelegate->getCookie();
  // starting a download is only allowed before a download!

  if (isdownloading) return false;
  /**/JDEBUGLOG(fname,2);
  if (afterdownload) return false;
  // we need a valid url
  if (!fetch_this.isValid()) return false;
  /**/JDEBUGLOG(fname,3);
  // building the request
  req = new QNetworkRequest(fetch_this);
  // for the request, there are some default values we have to overriden
  req->setRawHeader("User-Agent", "qjaffis-agent/1.1");
  req->setAttribute(QNetworkRequest::CacheSaveControlAttribute,false);
  req->setAttribute(QNetworkRequest::CookieSaveControlAttribute,true);
  // adding a cookie if needed
  if (!cookie.isEmpty()) {
    /* note that cookies are supposed to be ASCII, but Qt has no US-ASCII codec (the
      toAscii() function does not return us-ascii according to the Qt docs!). I'll
      risk latin-1, at least for now. */
    cookie_value = cookie.toLatin1();
    req->setRawHeader("cookie",cookie_value);
  }
  /**/JDEBUGLOG(fname,4);
  // now we build the manager
  if (intermediary == NULL) intermediary = new QNetworkAccessManager();
  sslErrors2Ignore.append(QSslError(QSslError::NoError));
  /**/JDEBUGLOG(fname,5);
  // and get the reply object
  raw_result = intermediary->get(*req);
  raw_result->ignoreSslErrors();
  /**/JDEBUGLOG(fname,6);
  // done
  isdownloading = true;
  return true;
}
//--------------------------
// tries to get the data and convert it to a string. false if en error happened.
bool jfFetchPage::PrepareResults() {
  // variables
  const QString fname = "jfFetchPage::PrepareResults";
  QEventLoop blocker;
  bool rc;
  QUrl redtarg;
  QByteArray byteresult, header_content;
  // the most basic error : we have not even started!
  if (!isdownloading) return false;
  // I like synchronous IO for it's self-contained nature
  if (raw_result->isRunning()) {
    rc = connect(raw_result, SIGNAL(finished()), &blocker, SLOT(quit()));
    assert(rc);
    blocker.exec();
  }
  // here, things are presumably finished
  assert(raw_result->isFinished());
  if (MakeError()) {
    // even if we got no error, we could still have redirection...
    redtarg = (raw_result->attribute(QNetworkRequest::RedirectionTargetAttribute)).toUrl();
    if ((!redtarg.isEmpty()) && (redtarg!=fetch_this)) {
      theerror = jff_REDIRECTION;
      /**/JDEBUGLOGS(fname,1,redirectto)
      redirectto = redtarg.toString();
    }
    // no server side redirection
    else {
      /* Getting the actual data at last. The following method gets the raw bytes,
      and the character set, converts it to the result, and optionally tests for
      client-side redirection via META tag. */
      if (ProcessResult()) {
        /**/JDEBUGLOG(fname,2)
        // finally, we use the function pointers to test the result
        // test missing
        if (testsDelegate != NULL) {
          rc = testsDelegate->testMissing(thepage);
          if (!rc) {
            /**/JDEBUGLOG(fname,3)
            theerror = jff_MISSING;
            delete thepage;
            thepage = NULL;
          }
        }
        // test incomplete
        if ((testsDelegate != NULL) && (thepage!=NULL)) {
          rc = testsDelegate->testIncomplete(thepage);
          if (!rc) {
            /**/JDEBUGLOGS(fname,4,"PAGE TRUNCATED")
            theerror = jff_TRYAGAIN;
            delete thepage;
            thepage = NULL;
          }
        }
        /**/JDEBUGLOGS(fname,5,jf_FetchErr2String(theerror,false));
      }
    }
  }
  // following, whether in error or not, we clean up...
  rc = ClearObjects();
  assert(rc);
  errlog[theerror]++;
  /**/JDEBUGLOGS(fname,6,"Finished")
  return (theerror==jff_NOERROR);
}
//--------------------------
// only used between calling the above two, halts download and clears download objects
bool jfFetchPage::Abort() {
  if (!isdownloading) return false;
  raw_result->abort();
  theerror = jff_TRYAGAIN;
  return ClearObjects();
}
//++++++++++++++++++++++++++++++++++++
// status info
//--------------------------
// true if the url exists and is valid
bool jfFetchPage::ValidUrl() const {
  return fetch_this.isValid();
}
//--------------------------
// true inbetween a succesful call to StartDownload(), and calling PrepareResults()
bool jfFetchPage::IsDownloading() const {
  return isdownloading;
}
//--------------------------
// true after calling PrepareResults(), before Reset()
bool jfFetchPage::AfterDownload() const {
  return afterdownload;
}
//-------------------------
size_t jfFetchPage::GetErrorCount(jfFETCH_ERROR pick_which) {
  return errlog[pick_which];
}
//++++++++++++++++++++++++++++++++++++
// result info
//--------------------------
// true if thepage (result) is not NULL, we have a string
bool jfFetchPage::HasResult() const {
  return (thepage!=NULL);
}
//--------------------------
// if successful, returns the page and Resets the Results
QString* jfFetchPage::GetResult() {
  QString* tempval;
  if (HasResult()) {
    tempval = thepage;
    thepage = NULL;
    Reset();
    return tempval;
  }
  else return NULL;
}
//--------------------------
// returns the error value
jfFETCH_ERROR jfFetchPage::GetError() const {
  return theerror;
}
//--------------------------
// returns the redirection result, which may be empty
QString jfFetchPage::GetRedirection() const {
  return redirectto;
}
//-------------------------
// like GetRedirection, but resolves the redirection first
QString jfFetchPage::GetRedirectTarget() const {
  QUrl relative,result;
  if (!redirectto.isEmpty()) {
    relative.setUrl(redirectto);
    result = fetch_this.resolved(relative);
    return result.toString();
  }
  else return "";
}
//--------------------------
// after calling PrepareResults(), one can call this to discard results
bool jfFetchPage::Reset() {
  if (!afterdownload) return false;
  if (thepage!=NULL) {
    delete thepage;
    thepage = NULL;
  }
  theerror = jff_NOERROR;
  redirectto.clear();
  afterdownload = false;
  return true;
}
//++++++++++++++++++++++++++++++++++++
// destructor
jfFetchPage::~jfFetchPage() {
  if (isdownloading) Abort();
  else Reset();
  delete intermediary;
}
//++++++++++++++++++++++++++++++++++++++++++++++++
// --- [ PROTECTED METHODS for jfFetchPage ] ---
//--------------------------
// internal methods
//--------------------------
// sets theerror based upon QNetworkReply::NetworkError
bool jfFetchPage::MakeError() {
  const QString fname = "jfFetchPage::MakeError";
  // special case
  if (!isdownloading) return false;
  // the source error
  /**/JDEBUGLOG(fname,1)
  QNetworkReply::NetworkError raw_error = raw_result->error();
  if (raw_error == QNetworkReply::NoError) {
    theerror = jff_NOERROR;
    return true;
  }
  /**/JDEBUGLOGS(fname,2,raw_result->errorString());
  /**/JDEBUGLOGST(fname,3,raw_error);
  // here, we have an actual error message to sort
  // one of the few times I've taken advantage of the C switch
  switch (raw_error) {
    case QNetworkReply::ConnectionRefusedError        :/**/JDEBUGLOG(fname,4);
    case QNetworkReply::TimeoutError                  :/**/JDEBUGLOG(fname,5);
    case QNetworkReply::RemoteHostClosedError         :/**/JDEBUGLOG(fname,6);
    case QNetworkReply::OperationCanceledError        :/**/JDEBUGLOG(fname,7);
    case QNetworkReply::TemporaryNetworkFailureError  :/**/JDEBUGLOG(fname,8);
    case QNetworkReply::ProxyConnectionRefusedError   :/**/JDEBUGLOG(fname,9);
    case QNetworkReply::ProxyConnectionClosedError    :/**/JDEBUGLOG(fname,10);
    case QNetworkReply::ProxyTimeoutError             :/**/JDEBUGLOG(fname,11);
    case QNetworkReply::ContentReSendError            :/**/JDEBUGLOG(fname,12);
    case QNetworkReply::UnknownNetworkError           :/**/JDEBUGLOG(fname,13);
    case QNetworkReply::UnknownProxyError             :/**/JDEBUGLOG(fname,14);
    case QNetworkReply::UnknownContentError           :/**/JDEBUGLOG(fname,15);
    case QNetworkReply::ProtocolFailure               :/**/JDEBUGLOG(fname,16);
    case QNetworkReply::ProtocolUnknownError          :/**/JDEBUGLOG(fname,17);
      theerror = jff_TRYAGAIN;
      break;
    case QNetworkReply::ContentNotFoundError :
      theerror = jff_MISSING;
      break;
    default :
      theerror = jff_FALIURE;
  }
  // done
  return false;
}
//---------------------------------
// read and convert the BytesRead, and check for redirection, etc.
bool jfFetchPage::ProcessResult() {
  const QString fname = "jfFetchPage::ProcessResult";
  // special case
  if (!isdownloading) return false;
  // internal variables
  QByteArray byteresult, content_header;
  QString charset, buffer;
  QTextCodec *htmlcodec, *defaultcodec, *win1252codec;
  // reading the raw results
  content_header = raw_result->rawHeader("Content-Type");
  byteresult = raw_result->readAll();
  // this should not really happen...
  if (byteresult.isEmpty()) {
    theerror = jff_TRYAGAIN;
    return false;
  }
  // character set conversion. Note that if unrecognized, we use UTF-8
  defaultcodec = QTextCodec::codecForName("UTF-8");
  htmlcodec = QTextCodec::codecForHtml(byteresult,defaultcodec);
  /**/JDEBUGLOGS(fname,1,htmlcodec->name());
  thepage = new QString();
  (*thepage) = htmlcodec->toUnicode(byteresult);
  /**/JDEBUGLOGB(fname,2,check_clientred);
  // optional ... check for client side redirection (meta version)
  if (check_clientred) {
      /**/JDEBUGLOG(fname,3);
    if (ParseForClient()) {
        /**/JDEBUGLOG(fname,4);
      theerror = jff_REDIRECTION;
      delete thepage;
      thepage = NULL;
      return false;
    }
  }
  // trying to get around treating Windows-1252 as UTF-8
  if (thepage->contains(QChar(0xFFFD))) {  // the presence of 'Unicode Replacement Characters' indicates problems
      // the above probably happens if Windows-1252 is mislabelled as utf-8
      bool checkbyte;
      // check for certain bytes
      checkbyte = byteresult.contains((char)133) || byteresult.contains((char)147);
      if (!checkbyte) checkbyte = byteresult.contains((char)148) || byteresult.contains((char)145);
      if (!checkbyte) checkbyte = byteresult.contains((char)146);
      // if the bytes are in, we treat the bytes as windows-1252
      win1252codec = QTextCodec::codecForName("windows-1252");
      (*thepage) = win1252codec->toUnicode(byteresult);
  }
  /**/JDEBUGLOG(fname,5);
  // done
  return true;
}
//-----------------------------------------------
// parses the result for client side redirection
bool jfFetchPage::ParseForClient() {
  // variables
  QString buffer, exturl;
  int strpos, endpos;
  // basic task
  if (thepage==NULL) return false;
  // starting
  strpos = thepage->indexOf("<META HTTP-EQUIV=\"refresh\"",0,Qt::CaseInsensitive);
  if (strpos==-1) return false;
  strpos += 26;
  endpos = thepage->indexOf(">",strpos);
  if (endpos == -1) return false;
  // extracting and then whitting things down to just the content
  buffer = thepage->mid(strpos,endpos-strpos);
  strpos = buffer.indexOf("CONTENT=",0,Qt::CaseInsensitive);
  if (strpos == -1) return false;
  strpos += 8;
  endpos = buffer.indexOf("\"",strpos);
  if (endpos == -1) return false;
  buffer = buffer.mid(strpos,endpos-strpos);
  buffer = buffer.trimmed();
  if (buffer.isEmpty()) return false;
  // the actual url should be found after the ;
  strpos = buffer.indexOf(';');
  if (strpos == -1) return false;
  buffer = buffer.mid(strpos+1);
  buffer = buffer.trimmed();
  if (buffer.isEmpty()) return false;
  // finally...
  redirectto = buffer;
  return true;
}
//--------------------------------------------------
// used to delete the qt objects after download
bool jfFetchPage::ClearObjects() {
  // invalid cases
  if (!isdownloading) return false;
  if (raw_result->isRunning()) return false;
  // deleting
  raw_result->close();
  raw_result->deleteLater();
  delete req;
  req = NULL;
  // setting flags
  isdownloading = false;
  afterdownload = true;
  raw_result = NULL;
  // done
  return true;
}

//*****************************************************************************
