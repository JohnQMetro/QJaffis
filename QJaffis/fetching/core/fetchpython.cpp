/*****************************************************************************
Name    : fetchpython.cpp
Basic   : An implementation of jfPageFetchInterface using an external Python
          script.
Author  : John Q Metro
Started : June 27, 2021
Updated : June 18, 2022

******************************************************************************/
#ifndef FETCHPYTHON_H
    #include "fetchpython.h"
#endif // FETCHPYTHON_H
#ifndef LOGGING_H_INCLUDED
  #include "../../core/utils/logging.h"
#endif // LOGGING_H_INCLUDED
#ifndef UTILS3_H_INCLUDED
  #include "../../core/utils/utils3.h"
#endif // UTILS3_H_INCLUDED
//----------------------
#include <QCoreApplication>
#include <QRegularExpression>
/*****************************************************************************/

jfPageFetchWithPython::jfPageFetchWithPython(jfPythonPaths* python_paths):jfPageFetchInterface() {
    const QString fname = "jfPageFetchWithPython::jfPageFetchWithPython";

    // path parameter checking
    bool okay_param = (python_paths != NULL) && python_paths->IsValid() && python_paths->ScriptExists();
    jerror::AssertLog(okay_param, fname, "Parameter paths are bad!");
    pythonpaths = python_paths;

    // other stuff
    script_active = false;
    script_com = NULL;
}
//+++++++++++++++++++++++++
    // implemented public methods
jfFETCH_ERROR jfPageFetchWithPython::Download(const QString& inurl, const QString& cookie) {
    const QString fname = "jfPageFetchWithPython::Download";
    // pre-fetch checks
    pfilock.lock();
    jfFETCH_ERROR startc = PreDownloadChecks(inurl);
    if (startc != jff_NOERROR) {
        pfilock.unlock();
        return startc;
    }
    // starting script (might already be started)
    bool okay = StartScript();
    if (!okay) {
        ClearOld();
        theerror = jff_FALIURE;
        startc = jff_FALIURE;
        pfilock.unlock();
        return startc;
    }
    // proceed with downloading
    isdownloading = true;
    pfilock.unlock();
    QString signal_result = ScriptFetch(cookie);

    // checking the result
    pfilock.lock();
    ClearOld();
    // the page has been successfully downloaded
    if (signal_result == "Y") {
        bool okay = LoadSavedPage();
        if (!okay) jerror::Log(fname, "Failed to load python-fetched page.");
    }
    // the page has failed to download
    else if (signal_result == "N") {
        bool eload = LoadErrors();
        if (!eload) jerror::Log(fname, "Failed to load python-script errors.");
    }
    // error happened inside ScriptFetch
    else if (signal_result == "") {
        // we have already aborted at this point
        theerror = jff_INTERNAL;
    }
    // the script produced unexpected output
    else {
        QByteArray readerr = script_com->readAllStandardError();
        AbortScript();
        jerror::Log(fname,"The script returned unexpected data:\n" + signal_result + "\n" + QString(readerr));
        theerror = jff_INTERNAL;
    }
    jfFETCH_ERROR temp = theerror;
    isdownloading = false;
    pfilock.unlock();
    return temp;
}
//---------------
bool jfPageFetchWithPython::Abort() {
    QMutexLocker mlock(&pfilock);
    if (script_active) AbortScript();
    isdownloading = false;
    theerror = jff_HALTED;
    return true;
}
//----------------------------
void jfPageFetchWithPython::Shutdown() {
    QMutexLocker mlock(&pfilock);
    if (script_active) {
        script_com->write("close\n");
        if (!(script_com->waitForFinished(1000))) {
            script_com->kill();
            script_com->waitForFinished();
        }
        script_com->deleteLater();
        script_active = false;
    }
    if (isdownloading) {
        isdownloading = false;
        theerror = jff_HALTED;
    }
    script_com = NULL;

}
//-------------------------------
jfPageFetchWithPython::~jfPageFetchWithPython() {
    AbortScript();
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++
QString jfPageFetchWithPython::ConvertErr(QProcess::ProcessError err) const {
    switch (err) {
    case QProcess::FailedToStart : return "0: The process failed to start";
    case QProcess::Crashed       : return "1: The process crashed some time after starting successfully.";
    case QProcess::Timedout      : return "2: The last waitFor...() function timed out.";
    case QProcess::WriteError    : return "4: An error occurred when attempting to write to the process.";
    case QProcess::ReadError     : return "3: An error occurred when attempting to read from the process.";
    case QProcess::UnknownError  : return "5: An unknown error occurred";
    default: return "Unknown QProcess error.";
    }
}
//------------------------------
void jfPageFetchWithPython::AbortScript() {
    if (script_com != NULL) {
        script_com->kill();
        script_com->waitForFinished();
        script_com->deleteLater();
        script_com = NULL;
    }
    script_active = false;
}
//-----------------------------
// used to setup the QProcess and the script
bool jfPageFetchWithPython::StartScript() {
    const QString fname = "jfPageFetchWithPython::StartScript";
    if (script_active) return true; // already done
    script_com = new QProcess();
    QStringList arg;
    // argument is just the script path and the transfer filename
    arg.append(pythonpaths->ScriptPath(false));
    arg.append(pythonpaths->TransferPath(true));
    script_com->setProcessChannelMode(QProcess::ForwardedErrorChannel);
    // start and wait max 10s for startup...
    script_com->start(pythonpaths->InvokeCmd(false),arg);
    if(!script_com->waitForStarted(10000)) {
        // handling failed to start
        QString err = ConvertErr(script_com->error());
        jerror::Log(fname,"Failed " + err);
        AbortScript();
        return false;
    }
    // we read the startup message
    bool okay = script_com->waitForReadyRead(180000); // max 3 min wait
    if (!okay) {
        QString err = ConvertErr(script_com->error());
        jerror::Log(fname,"waiting for process to return a result failed! " + err);
        AbortScript();
        return false;
    }
    QByteArray data = (script_com->readLine()).trimmed();
    if (data.isEmpty()) {
        QString err = ConvertErr(script_com->error());
        jerror::Log(fname,"reading the startup message did not give results! " + err);
        AbortScript();
        return false;
    }
    script_active = true;

    return true;
}
//------------------------------------
/* tells the script to fetch a page and waits for the response,
 * handles any errors. */
QString jfPageFetchWithPython::ScriptFetch(const QString& cookie) {
    const QString fname = "jfPageFetchWithPython::ScriptFetch";
    jerror::AssertLog(script_active, fname, "Script is not active!");
    // making info to send
    QByteArray tosend = fetch_this.toEncoded();
    if (!cookie.isEmpty()) tosend += " ||| " + cookie.toLatin1() + "\n";
    else tosend += "\n";
    // sending
    /* qint64 writeres =*/ script_com->write(tosend);

    bool okay = script_com->waitForBytesWritten(3000);
    if (!okay) {
        QString err = ConvertErr(script_com->error());
        jerror::Log(fname,"writing to process failed! " + err);
        AbortScript();
        return "";
    }
    // reading
    int waitsdone = 0;
    while (waitsdone < 30) {
       okay = script_com->waitForReadyRead(30000); // 30 sec wait
       if (script_com->bytesAvailable() > 0) break;
       waitsdone++;
    }
    if (script_com->bytesAvailable() == 0) {
        QString err = ConvertErr(script_com->error());
        jerror::Log(fname,"waiting for process to return a result failed! " + err);
        AbortScript();
        return "";
    }
    QByteArray data = (script_com->readAllStandardOutput()).trimmed();
    // QByteArray data = (script_com->readLine()).trimmed();
    if (data.isEmpty()) {
        QString err = ConvertErr(script_com->error());
        jerror::Log(fname,"reading did not return any results!! " + err);
        AbortScript();
        return "";
    }
    return QString(data);
}
//------------------------------------------
/* When this is called, the download page is presumed to be stored in a file. */
bool jfPageFetchWithPython::LoadSavedPage() {
    const QString fname = "jfPageFetchWithPython::LoadSavedPage";
    QString data;
    bool read_okay = FromFile(data,pythonpaths->TransferPath(false));
    if (read_okay) {
        thepage = new QString();
        (*thepage) = data;
        theerror = jff_NOERROR;
        return true;
    }
    else {
        jerror::Log(fname,"Failed to load the file " + pythonpaths->TransferPath(true));
        theerror = jff_INTERNAL;
        return false;
    }
}
//------------------------------------------------
jfFETCH_ERROR jfPageFetchWithPython::ConvertStatusCode(int http_status) const {
    // these should never happen, so treat as a temporary glitch
    if (http_status < 100) return jff_TRYAGAIN; // out of range
    else if (http_status < 200) return jff_TRYAGAIN; // not finished yet
    // okay
    else if ((http_status == 200) || (http_status == 201)) return jff_NOERROR;
    // strange
    else if (http_status < 300) return jff_TRYAGAIN;
    // redirection
    else if (http_status < 400) return jff_REDIRECTION;
    // rate limit
    else if ((http_status == 429) || (http_status == 503)) return jff_RATELIMIT;
    // missing
    else if ((http_status == 404) || (http_status == 410) || (http_status == 451)) {
            return jff_MISSING;
    }
    // try again
    else if ((http_status == 406) || (http_status == 408) || (http_status == 409) ||
             (http_status == 418) || (http_status == 421) || (http_status == 423) ||
             (http_status == 425) || (http_status == 500) || (http_status == 502) ||
             (http_status == 503) || (http_status == 504) || (http_status == 507) ||
             (http_status == 508)) return jff_TRYAGAIN;
    // hard failure
    else return jff_FALIURE;

}
//------------------------------------------------
/* when the script reports an error, the file should contain error messages. */
bool jfPageFetchWithPython::LoadErrors() {
    const QString fname = "jfPageFetchWithPython::LoadErrors";
    QString errdata;
    bool read_okay = FromFile(errdata,pythonpaths->TransferPath(false));
    int http_status = -1;
    /**/JDEBUGLOGB(fname,1,read_okay)
    if (read_okay) {
        QStringList errList = errdata.split(QRegularExpression("\\R"),QString::SkipEmptyParts);
        // first line
        if ((errList.count() > 0) && (!errList.at(0).isEmpty())) {
            bool ok;
            http_status = errList.at(0).toInt(&ok);
            if (!ok) {
                jerror::Log(fname,"First line of error file could not be converted to an integer (status code)!");
                theerror = jff_INTERNAL;
                return false;
            }
        }
        else {
            jerror::Log(fname,"First line of error file missing or empty!");
            theerror = jff_INTERNAL;
            return false;
        }
        // converting status code to a simplified error
        /**/JDEBUGLOGI(fname,2,http_status)
        jfFETCH_ERROR firsterror = ConvertStatusCode(http_status);
        // getting the second line if there is one
        if (errList.count() >= 2) {
            QString errdesc = errList.at(1);
            /**/JDEBUGLOGS(fname,3,errdesc);
        }
        // some special cases
        if (firsterror == jff_NOERROR) {
            jerror::Log(fname,"No Error status code in error processing!");
            theerror = jff_INTERNAL;
            return false;
        }
        // special handling
        else if ((firsterror == jff_REDIRECTION) || (firsterror == jff_RATELIMIT)) {
            if ((errList.count() < 3) || (errList.at(2).isEmpty())) {
                jerror::Log(fname,"Redirection or rate limit missing extra info!");
                theerror = jff_INTERNAL;
                return false;
            }
            QString errinfo = errList.at(2).trimmed();
            /**/JDEBUGLOGS(fname,4,errinfo);
            // redirection
            if (firsterror == jff_REDIRECTION) {
                redirectto = errinfo;
                /**/JDEBUGLOGS(fname,5,redirectto);
            }
            // rate limit
            else {
                bool oint;
                int retry_after = errinfo.toInt(&oint);
                if (!oint) limit_wait_time = -1;
                else limit_wait_time = retry_after;
                /**/JDEBUGLOGI(fname,6,limit_wait_time);
            }
            theerror = firsterror;
            return true;
        }
        // anothing else
        else {
            /**/JDEBUGLOG(fname,7);
            theerror = firsterror;
            return true;
        }
    }
    else {
        jerror::Log(fname,"Failed to load the error file " + pythonpaths->TransferPath(true));
        theerror = jff_INTERNAL;
        return false;
    }
}

/*****************************************************************************/
