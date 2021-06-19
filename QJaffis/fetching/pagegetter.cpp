/*****************************************************************************
Name    : pagegetter.cpp
Basic   : Defines my own html page fetching class (new for Qt)
Author  : John Q Metro
Started : March 16, 2013
Updated : January 2, 2021
Started attempts to get past Cloudflare

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

#ifndef CLOUDFGET_H
  #include "cloudfget.h"
#endif // CLOUDFGET_H

#include <assert.h>
#include <random>
#include <ctime>
//---------------------------------
#include <QByteArray>
#include <QEventLoop>
#include <QTextCodec>
#include <QSslError>
#include <QCoreApplication>
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
  use_cloudscrape = false;

  // attempts to get past archive of our own rate limiting failed
  /*
  limit_count = 50;
  xff_ip = "192.168.10.5";
  xfh = (", X-Forwarded-For:" + xff_ip).toAscii();
  rlimd.append("archiveofourown.org");
  rlimd.append("www.archiveofourown.org");
  srand(time(NULL));
  hs = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/85.0.4183.";
  xhq = (hs + jfFetchPage::randomByte() + " Safari/537.36").toAscii();
  */
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
  use_cloudscrape = (fetch_this.isValid() && (fetch_this.host() == "www.fanfiction.net"));
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
  /**/JDEBUGLOGB(fname,3,use_cloudscrape);

  /* Trying to use tokens from cloudscaper is not working, so
   * use the cloudscraper script to do the entire download... */
  if (use_cloudscrape) {
      thepage = cloudscrape::ffn_interface->FetchOnePage(fetch_this);
      isdownloading = true;
      return true;
  }

  // building the request
  req = new QNetworkRequest(fetch_this);
  req->setAttribute(QNetworkRequest::CacheSaveControlAttribute,false);
  req->setAttribute(QNetworkRequest::CookieSaveControlAttribute,true);
  /**/JDEBUGLOG(fname,4);
  /**/JDEBUGLOG(fname,6);
  req->setRawHeader("User-Agent", "qjaffis-agent/1.4");
  if (!cookie.isEmpty()) {
    /* note that cookies are supposed to be ASCII, but Qt has no US-ASCII codec (the
       toAscii() function does not return us-ascii according to the Qt docs!). I'll
       risk latin-1, at least for now. */
    cookie_value = cookie.toLatin1();
    req->setRawHeader("cookie",cookie_value);
  }
  /* commenting out the failed attempt to get past AO3 rate limiting
  req->setRawHeader("User-Agent", xhq);
  RateLimitHandle(*req);
  */

  /**/JDEBUGLOG(fname,7);
  // now we build the manager
  if (intermediary == NULL) intermediary = new QNetworkAccessManager();
  sslErrors2Ignore.append(QSslError(QSslError::NoError));
  /**/JDEBUGLOG(fname,8);
  // and get the reply object
  raw_result = intermediary->get(*req);
  raw_result->ignoreSslErrors();
  /**/JDEBUGLOG(fname,9);
  // done
  isdownloading = true;
  return true;
}
//--------------------------
// tries to get the data and convert it to a string. false if en error happened.
bool jfFetchPage::PrepareResults() {
  // variables
  const QString fname = "jfFetchPage::PrepareResults";
  // the most basic error : we have not even started!
  /**/JDEBUGLOG(fname,1)
  if (!isdownloading) return false;
  /* using cloudscaper, the download is already done, so we have some dummy stuff */
  if (use_cloudscrape) {
      /**/JDEBUGLOG(fname,2)
      // no complicated error handling here
      bool okay = (thepage != NULL);
      theerror = (okay) ? jff_NOERROR : jff_FALIURE;
      if (okay) okay = TestPageDelegates();
      // after fetch
      /**/JDEBUGLOG(fname,3)
      isdownloading = false;
      afterdownload = true;
      raw_result = NULL;

      errlog[theerror]++;
      /**/JDEBUGLOGS(fname,4,"Finished")
      return (theerror==jff_NOERROR);
  }
  /**/JDEBUGLOG(fname,5)
  // otherwise, the async fetch might still be running
  QEventLoop blocker;
  bool rc;
  QUrl redtarg;
  QByteArray byteresult, header_content;
  /**/JDEBUGLOG(fname,6)
  // I like synchronous IO for it's self-contained nature
  if (raw_result->isRunning()) {
    rc = connect(raw_result, SIGNAL(finished()), &blocker, SLOT(quit()));
    assert(rc);
    blocker.exec();
  }
  /**/JDEBUGLOG(fname,7)
  // here, things are presumably finished
  assert(raw_result->isFinished());
  if (MakeError()) {
    // even if we got no error, we could still have redirection...
    redtarg = (raw_result->attribute(QNetworkRequest::RedirectionTargetAttribute)).toUrl();
    if ((!redtarg.isEmpty()) && (redtarg!=fetch_this)) {
      theerror = jff_REDIRECTION;
      /**/JDEBUGLOGS(fname,8,redirectto)
      redirectto = redtarg.toString();
    }
    // no server side redirection
    else {
      /* Getting the actual data at last. The following method gets the raw bytes,
      and the character set, converts it to the result, and optionally tests for
      client-side redirection via META tag. */
      if (ProcessResult(false)) {
        /**/JDEBUGLOG(fname,9)
        // finally, we use the function pointers to test the result
        TestPageDelegates();
      }
    }
  }
  else if (theerror == jff_RATELIMIT) {
      QByteArray ret = raw_result->rawHeader("Retry-After");
      QString rastr = QString(ret).trimmed();
      /**/JDEBUGLOGS(fname,10,"RETRY-AFTER: " + rastr);
      bool oint;
      retry_after = rastr.toInt(&oint);
      if (!oint) retry_after = 0;
  }
  else {
      ProcessResult(false);
  }
  // following, whether in error or not, we clean up...
  rc = ClearObjects();
  assert(rc);
  errlog[theerror]++;
  /**/JDEBUGLOGS(fname,11,"Finished")
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
  // limit_count = 50;
  return true;
}
//-----------------------------------
int jfFetchPage::GetRetryAfter() const {
    return retry_after;
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
bool jfFetchPage::TestPageDelegates() {
    const QString fname = "jfFetchPage::TestPageDelegates";
    /**/JDEBUGLOG(fname,1)
    // if the tests delegate is missing, is is considered true by default
    if (testsDelegate == NULL) return true;
    // test missing
    bool rc = testsDelegate->testMissing(thepage);
    /**/JDEBUGLOGB(fname,2,rc)
    if (!rc) {
        /**/JDEBUGLOGS(fname,3,"FAILED MISSING TEST")
        theerror = jff_MISSING;
        delete thepage;
        thepage = NULL;
        return false;
    }
    // test incomplete
    /**/JDEBUGLOG(fname,4)
    rc = testsDelegate->testIncomplete(thepage);
    /**/JDEBUGLOGB(fname,5,rc)
    if (!rc) {
        /**/JDEBUGLOGS(fname,6,"PAGE TRUNCATED")
        /**/JDEBUGLOGS(fname,7,(*thepage))
        theerror = jff_TRYAGAIN;
        delete thepage;
        thepage = NULL;
        return false;
    }
    /**/JDEBUGLOGS(fname,8,jf_FetchErr2String(theerror,false));
    return true;
}

//--------------------------
// internal methods
//--------------------------
// sets theerror based upon QNetworkReply::NetworkError
bool jfFetchPage::MakeError() {
  const QString fname = "jfFetchPage::MakeError";
  QString serror;
  // special case
  if (!isdownloading) return false;
  // the source error
  /**/JDEBUGLOG(fname,1)
  QNetworkReply::NetworkError raw_error = raw_result->error();
  if (raw_error == QNetworkReply::NoError) {
    theerror = jff_NOERROR;
    return true;
  }
  serror = raw_result->errorString();
  /**/JDEBUGLOGS(fname,2,serror);
  /**/JDEBUGLOGST(fname,3,raw_error);
  // here, we have an actual error message to sort
  // one of the few times I've taken advantage of the C switch
  switch (raw_error) {
    case QNetworkReply::UnknownContentError           :/**/JDEBUGLOG(fname,15);
      if (serror.contains("Too Many Requests",Qt::CaseInsensitive)) {
          theerror = jff_RATELIMIT;
          // limit_count = 50;
      }
      else theerror = jff_FALIURE;
      break;
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
bool jfFetchPage::ProcessResult(bool write_to_file) {
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
  /**/JDEBUGLOG(fname,1)
  if (byteresult.isEmpty()) {
      /**/JDEBUGLOG(fname,2)
    theerror = jff_TRYAGAIN;
    return false;
  }
  // character set conversion. Note that if unrecognized, we use UTF-8
  defaultcodec = QTextCodec::codecForName("UTF-8");
  htmlcodec = QTextCodec::codecForHtml(byteresult,defaultcodec);
  /**/JDEBUGLOGS(fname,3,htmlcodec->name());
  thepage = new QString();
  (*thepage) = htmlcodec->toUnicode(byteresult);
  // special, write to file for debugging...
  if (write_to_file) {
      /**/JDEBUGLOG(fname,4)
    QString fname = fetch_this.encodedPath() + "x" + ".html";
      fname = fname.replace("/","_");
      fname = "/" + fname;
    QString path= QCoreApplication::applicationDirPath() + fname;
    /**/JDEBUGLOGS(fname,4,path)
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) file.close();
    else {
        /**/JDEBUGLOG(fname,5)
      file.write(thepage->toUtf8());
      file.close();
    }
  }

  /**/JDEBUGLOGB(fname,6,check_clientred);
  // optional ... check for client side redirection (meta version)
  if (check_clientred) {
      /**/JDEBUGLOG(fname,7);
    if (ParseForClient()) {
        /**/JDEBUGLOG(fname,8);
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
  /**/JDEBUGLOG(fname,9);
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
/*
//-----------------------------------------------------------
bool jfFetchPage::RateLimitHandle(QNetworkRequest& head_target) {
    const QString fname = "jfFetchPage::RateLimitHandle";
    QString domain = fetch_this.host();
    if (!rlimd.contains(domain,Qt::CaseInsensitive)) return false;
    limit_count++;
    if (limit_count > 20) {
        xff_ip = jfFetchPage::randomByte() + ".";
        xff_ip += jfFetchPage::randomByte() + ".";
        xff_ip += jfFetchPage::randomByte() + ".";
        xff_ip += jfFetchPage::randomByte();
        limit_count = 1;

        xfh = xff_ip.toAscii();
        // xhq = (hs + jfFetchPage::randomByte() + " Safari/537.36").toAscii();
    }

    // head_target.setRawHeader("User-Agent", xhq);
    head_target.setRawHeader("X-Originating-IP", xfh);
    head_target.setRawHeader("X-Forwarded-For", "rwgaergwe");
    head_target.setRawHeader("X-Remote-IP", xfh);
    head_target.setRawHeader("X-Remote-Addr", xfh);
    return true;
}
*/
QString jfFetchPage::randomByte() {
    return QString::number(rand() % 256);
}

//*****************************************************************************
