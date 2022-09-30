/*****************************************************************************
Name    : fetchqt.cpp
Basic   : An implementation of jfPageFetchInterface using Qt downloaders
Author  : John Q Metro
Started : June 20, 2021
Updated : June 28, 2021

******************************************************************************/
#ifndef FETCHQT_H
    #include "fetchqt.h"
#endif // FETCHQT_H
//----------------------
#ifndef FETCHBASE_H
    #include "fetchbase.h"
#endif // FETCHBASE_H
#ifndef LOGGING_H_INCLUDED
    #include "../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED
//----------------------
#include <cstdlib>
#include <QEventLoop>
#include <climits>
/*****************************************************************************/
jfPageFetchWithQt::jfPageFetchWithQt(const QStringList* user_agents):jfPageFetchInterface() {
    // set to undefined
    // we will re-use the manager
    intermediary = new QNetworkAccessManager();
    raw_result = NULL;

    // character encoding stuff
    defaultcodec = QTextCodec::codecForName("UTF-8");
    win1252codec = QTextCodec::codecForName("windows-1252");

    unsigned char* temp = new unsigned char[wsize];
    temp[0] = (unsigned char)133u;  temp[1] = (unsigned char)147u;
    temp[2] = (unsigned char)148u;  temp[3] = (unsigned char)145u;
    temp[4] = (unsigned char)146u;
    w1252c = reinterpret_cast<char*>(temp);

    // setting up the user agents
    QByteArray current = QString("qjaffis-agent/2.0").toLatin1();
    QByteArray* bx = new QByteArray();
    (*bx) = current;
    default_user_agent = bx;

    if ((user_agents == NULL) || (user_agents->isEmpty())) {
        useragents = NULL;
    }
    else {
        useragents = new std::vector<const QByteArray*>();
        useragents->reserve(user_agents->count());
        for(int x = 0; x < user_agents->count(); x++) {
            current = (user_agents->at(x)).toLatin1();
            if (current.size() <= 0) continue;
            bx = new QByteArray();
            (*bx) = current;
            useragents->push_back(bx);
        }
        if (useragents->size() == 0) {
            delete useragents;
            useragents = NULL;
        }
    }

}
//-----------------------------------------
// implemented public methods
jfFETCH_ERROR jfPageFetchWithQt::Download(const QString& inurl, const QString& cookie) {
    const QString fname = "jfPageFetchWithQt::Download";
    // pre-fetch checks
    pfilock.lock();
    jfFETCH_ERROR startc = PreDownloadChecks(inurl);
    if (startc != jff_NOERROR) {
        pfilock.unlock();
        return startc;
    }
    // proceed with downloading
    isdownloading = true;
    QNetworkRequest* req = MakeRequest(cookie);
    raw_result = intermediary->get(*req);
    pfilock.unlock();

    raw_result->ignoreSslErrors();
    // I like synchronous IO for it's self-contained nature
    QEventLoop blocker;
    bool rc;
    if (raw_result->isRunning()) {
      rc = connect(raw_result, SIGNAL(finished()), &blocker, SLOT(quit()));
      assert(rc);
      blocker.exec();
    }

    // here, things are presumably finished
    jerror::AssertLog(raw_result->isFinished(), fname, "download not finished even after finishing!");
    pfilock.lock();
    ClearOld();
    bool okay = CheckInterpretError();
    // if we have no error, proceed to turning the result into a page
    if (okay) {
        okay = ProcessResult();
        if (!okay) theerror = jff_TRYAGAIN;
    }
    // cleaning up
    raw_result->deleteLater();
    raw_result = NULL;
    jfFETCH_ERROR temp = theerror;
    isdownloading = false;
    pfilock.unlock();
    return temp;

}
//------------------------------------
bool jfPageFetchWithQt::Abort() {
    QMutexLocker mlock(&pfilock);
    if ((raw_result == NULL) || (!(raw_result->isFinished()))) return false;
    raw_result->abort();
    return true;
}
//----------------------------------
void jfPageFetchWithQt::Shutdown() {

}
//+++++++++++++++++++++++++++++++++++++++++++++
jfPageFetchWithQt::~jfPageFetchWithQt() {
    if (raw_result != NULL) {
        if (raw_result->isRunning()) raw_result->abort();
        raw_result->deleteLater();
        raw_result = NULL;

    }
    if (useragents != NULL) {
        for (unsigned long x = 0; x < (useragents->size()); x++) {
            delete (useragents->at(x));
        }
        useragents->clear();
        delete useragents;
        useragents = NULL;
    }
    delete default_user_agent;
    default_user_agent = NULL;

    delete w1252c;
    w1252c = NULL;
    win1252codec = NULL;
    defaultcodec = NULL;

    intermediary->deleteLater();
    intermediary = NULL;

}
//+++++++++++++++++++++++++++++++++++++++++++++
const QByteArray* jfPageFetchWithQt::GetUserAgent() const {
    if (useragents == NULL) return default_user_agent;
    else {
        int udex = rand() % (useragents->size());
        return useragents->at(udex);
    }
}
//-------------------------------
QNetworkRequest* jfPageFetchWithQt::MakeRequest(const QString& cookie) const {
    QNetworkRequest* req = new QNetworkRequest(fetch_this);
    // cookie
    req->setAttribute(QNetworkRequest::CacheSaveControlAttribute,false);
    req->setAttribute(QNetworkRequest::CookieSaveControlAttribute,true);
    if (cookie != "") {
        req->setRawHeader("cookie",cookie.toLatin1());
    }
    // user agent
    const QByteArray* ua = GetUserAgent();
    req->setRawHeader("User-Agent", (*ua));
    // done
    return req;
}

//+++++++++++++++++++++++++++++++++++++++++++++
/* once we have the Qt fetch result object, we check for errors
 * and convert them to the simpler jf error code. */
bool jfPageFetchWithQt::CheckInterpretError() {
    const QString fname = "jfPageFetchWithQt::CheckInterpretError";
    // special case
    jerror::AssertLog(isdownloading,fname,"Make Error called when not downloading!");

    // the source error
    QNetworkReply::NetworkError raw_error = raw_result->error();
    // handling aborts quickly
    if (raw_error == QNetworkReply::OperationCanceledError) {
        theerror = jff_HALTED;
        return false;
    }

    QString serror = raw_result->errorString();
    /**/JDEBUGLOGS(fname,1,serror)
    QVariant status_code = raw_result->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    int http_status = -1;
    bool status_valid = status_code.canConvert<int>();
    if (status_valid) {
        http_status = status_code.toInt();
        /**/JDEBUGLOGI(fname,2,http_status);
    }

    // the 'no error case' might still mean a redirection error
    if (raw_error == QNetworkReply::NoError) {
        /**/JDEBUGLOG(fname,3)
        // server side redirection is 'NoError'
        QUrl redtarg;
        redtarg = (raw_result->attribute(QNetworkRequest::RedirectionTargetAttribute)).toUrl();
        if ((!redtarg.isEmpty()) && (redtarg!=fetch_this)) {
            theerror = jff_REDIRECTION;
            redirectto = redtarg.toString();
            /**/JDEBUGLOGS(fname,4,redirectto)
            return false;
        }
        else {
            /**/JDEBUGLOG(fname,5)
            theerror = jff_NOERROR;
            return true;
        }
    }
    // rate limit checking
    else if ((http_status == 429) || (http_status == 503)) {
        /**/JDEBUGLOG(fname,6)
        theerror = jff_RATELIMIT;
        QList<QByteArray> headerList = raw_result->rawHeaderList();
        foreach(QByteArray head, headerList) {
            QString headstr = "##" + head + ": " + raw_result->rawHeader(head);
            /**/JDEBUGLOGS(fname,6,headstr);
        }
        QByteArray ret = raw_result->rawHeader("Retry-After");
        QString rastr = QString(ret).trimmed();
        /**/JDEBUGLOGS(fname,7,"RETRY-AFTER: " + rastr);
        bool oint;
        int retry_after = rastr.toInt(&oint);
        if (!oint) limit_wait_time = -1;
        else limit_wait_time = retry_after;
        /**/JDEBUGLOGI(fname,8,limit_wait_time)
        return false;
    }
    /**/JDEBUGLOG(fname,9)
    // here, we have other errors to translate
    // one of the few times I've taken advantage of the C switch
    switch (raw_error) {
        case QNetworkReply::ConnectionRefusedError        :/**/JDEBUGLOG(fname,10)
        case QNetworkReply::TimeoutError                  :/**/JDEBUGLOG(fname,11)
        case QNetworkReply::RemoteHostClosedError         :/**/JDEBUGLOG(fname,12)
        case QNetworkReply::TemporaryNetworkFailureError  :/**/JDEBUGLOG(fname,14)
        case QNetworkReply::ProxyConnectionRefusedError   :/**/JDEBUGLOG(fname,15)
        case QNetworkReply::ProxyConnectionClosedError    :/**/JDEBUGLOG(fname,16)
        case QNetworkReply::ProxyTimeoutError             :/**/JDEBUGLOG(fname,17)
        case QNetworkReply::ContentReSendError            :/**/JDEBUGLOG(fname,18)
        case QNetworkReply::UnknownNetworkError           :/**/JDEBUGLOG(fname,19)
        case QNetworkReply::UnknownProxyError             :/**/JDEBUGLOG(fname,20)
        case QNetworkReply::ProtocolFailure               :/**/JDEBUGLOG(fname,21)
        case QNetworkReply::ProtocolUnknownError          :/**/JDEBUGLOG(fname,22)
        case QNetworkReply::ContentConflictError          :/**/JDEBUGLOG(fname,23)
        case QNetworkReply::InternalServerError           :/**/JDEBUGLOG(fname,24)
        case QNetworkReply::ServiceUnavailableError       :/**/JDEBUGLOG(fname,25)
        case QNetworkReply::UnknownServerError            :/**/JDEBUGLOG(fname,26)
            /**/JDEBUGLOGS(fname,28, serror);
            theerror = jff_TRYAGAIN;
            break;
        case QNetworkReply::ContentNotFoundError :
        case QNetworkReply::HostNotFoundError:
        case QNetworkReply::ContentGoneError:
            theerror = jff_MISSING;
            /**/JDEBUGLOGS(fname,30, serror);
            break;
        default :
            jerror::Log(fname,"Failed to download Error: " + serror);
            theerror = jff_FALIURE;
    }
    // done
    return false;
}
//--------------------------------
// determinining/guessing the download character encoding
QTextCodec* jfPageFetchWithQt::FigureOutCodec(const QByteArray& body_bytes) const {
    // looking inside the result page
    QTextCodec *htmlcodec = QTextCodec::codecForHtml(body_bytes, NULL);
    if (htmlcodec != NULL) {
        // recommended by W3C
        if ((htmlcodec->name()) == "ISO 8859-1") return win1252codec;
        else return htmlcodec;
    }
    // fallback 1, the content type header
    QByteArray content_header = raw_result->rawHeader("Content-Type");
    if (!content_header.isEmpty()) {
        // parsing a QByteArray directly is too awkwards
        QString content_type = QString(content_header);
        int ctd = content_type.indexOf("charset=",0,Qt::CaseInsensitive);
        if (ctd >= 0) {
            ctd += 8;
            int nx = content_type.indexOf(";",ctd);
            if (nx < 0) nx = content_type.length();
            QString csetp = content_type.mid(ctd,nx-ctd).trimmed();
            // converting the name
            htmlcodec = QTextCodec::codecForHtml(body_bytes, NULL);
        }
    }
    // final checks and fallbacks
    if (htmlcodec != NULL) {
        // recommended by W3C
        if ((htmlcodec->name()) == "ISO 8859-1") return win1252codec;
        else return htmlcodec;
    }
    else return defaultcodec;
}
//-------------
bool jfPageFetchWithQt::CheckWin1252(const QByteArray& body_bytes) const {
    char curr;
    for (int x = 0; x < body_bytes.length(); x++) {
        curr = body_bytes.at(x);
        // the skip depends on the char type
        #if (CHAR_MIN == 0)
        if (curr < 128) continue;
        #else
        if (curr >= 0) continue;
        #endif
        // however, the rest just compares against a pre-initialized array
        for (int y = 0; y < wsize; y++) {
            if (curr == w1252c[y]) return true;
        }
    }
    return false;
}
//--------------------------------
// read and convert the BytesRead
bool jfPageFetchWithQt::ProcessResult() {
    const QString fname = "jfPageFetchWithQt::ProcessResult";
    // special case
    jerror::AssertLog(isdownloading,fname,"Process Result called when not downloading!");
    // internal variables
    QByteArray byteresult;
    // QString charset, buffer;
    QTextCodec *codec;

    // reading the raw results
    byteresult = raw_result->readAll();
    // this should not really happen...
    /**/JDEBUGLOG(fname,1)
    if (byteresult.isEmpty()) {
        jerror::Log(fname,"Read in error");
        theerror = jff_TRYAGAIN;
        return false;
    }
    /**/JDEBUGLOG(fname,2);
    // getting the character set
    codec = FigureOutCodec(byteresult);
    /**/JDEBUGLOGS(fname,3,codec->name());
    // converting and producing the result
    if (thepage != NULL) delete thepage;
    thepage = new QString();
    (*thepage) = codec->toUnicode(byteresult);

    // trying to get around treating Windows-1252 as UTF-8
    // the presence of 'Unicode Replacement Characters' indicates problems
    if (thepage->contains(QChar(0xFFFD))) {
        /**/JDEBUGLOG(fname,4);
        // check for certain bytes
        if (CheckWin1252(byteresult)) {
            // reconvert as Windows-1252
            (*thepage) = win1252codec->toUnicode(byteresult);
            /**/JDEBUGLOG(fname,5);
        }
    }
    /**/JDEBUGLOG(fname,6);
    // done
    return true;
}



