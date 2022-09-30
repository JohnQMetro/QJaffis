/*****************************************************************************
Name    : pythonpaths.h
Basic   : Wraps and manipulates the file paths needed by the python downloader.
Author  : John Q Metro
Started : July 11, 2021
Updated : July 14, 2021

******************************************************************************/
#ifndef PYTHONPATHS_H
    #define PYTHONPATHS_H
#endif // PYTHONPATHS_H
//--------------------------------
#include <QString>
#include <QDir>
/*****************************************************************************/
/* the python fetcher required 3 file paths, and putting them together is more
complex than I would have liked. */

class jfPythonPaths {
  public:
    jfPythonPaths(const QString invoke_command, const QString script_filename, const QString transfer_filename, const QDir* base_path = NULL);
    // output
    bool IsValid() const;
    bool DirectorySet() const;
    QString InvokeCmd(bool checkquote) const;
    QString ScriptPath(bool checkquote) const;
    QString TransferPath(bool checkquote) const;
    bool ScriptExists() const;
    // changing
    bool SetTransferName(const QString& new_name);
    // output
    jfPythonPaths* CopyWithNewTransferName(const QString& new_name) const;
    virtual ~jfPythonPaths();

  protected:
    QString MakeFullPath(QString start, const QDir* base_folder) const;
    bool CheckNonEmpty() const;
    QString invoke;
    QString fname_script;
    QString fname_transfer;
    QString script_path;
    QString transfer_path;
    const QDir* basepath;
    bool is_valid;
    bool dirSet;
};
/*****************************************************************************/
