/*****************************************************************************
Name    : fetchpython.h
Basic   : An implementation of jfPageFetchInterface using an external Python
          script.
Author  : John Q Metro
Started : June 27, 2021
Updated : May 14, 2022

******************************************************************************/
#ifndef FETCHPYTHON_H
    #define FETCHPYTHON_H
#endif // FETCHPYTHON_H
//----------------------
#ifndef FETCHBASE_H
    #include "fetchbase.h"
#endif // FETCHBASE_H

#ifndef PYTHONPATHS_H
    #include "pythonpaths.h"
#endif // PYTHONPATHS_H
//----------------------
#include <QProcess>
#include <QDir>
/***************************************************************************/ /*
Uses QProcess to start an external python script, which then waits on input.
To download, the line <url> ||| <cookie> is passed via input to the script.
When done, the script should write results to the transfer file (to avoid character
encoding problems). The script then returns Y or N on the output. 'Y' means
the file contains the downloaded page. 'N' means it contains error info.
Error Format: (per line)
HTTP Status code
Text Error
Redirection or Retry After

sending anything not starting in http or https would be a sign for the script to quit

*//***************************************************************************/

class jfPageFetchWithPython : public virtual jfPageFetchInterface {
  public:

    jfPageFetchWithPython(jfPythonPaths* python_paths);

    // implemented public methods
    virtual jfFETCH_ERROR Download(const QString& inurl, const QString& cookie = "");
    virtual bool Abort();
    virtual void Shutdown();

    ~jfPageFetchWithPython();

  protected:
    QString ConvertErr(QProcess::ProcessError err) const;
    void AbortScript();
    bool StartScript();
    QString ScriptFetch(const QString& cookie);
    bool LoadSavedPage();
    jfFETCH_ERROR ConvertStatusCode(int http_status) const;
    bool LoadErrors();

    // script interface
    QProcess* script_com;
    jfPythonPaths* pythonpaths;
    bool script_active;

    // flags and other misc data
    int limit_wait_time;

};

/*****************************************************************************/
