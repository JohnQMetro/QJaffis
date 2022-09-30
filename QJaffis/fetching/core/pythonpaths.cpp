/*****************************************************************************
Name    : pythonpaths.cpp
Basic   : Wraps and manipulates the file paths needed by the python downloader.
Author  : John Q Metro
Started : July 11, 2021
Updated : May 28, 2022

******************************************************************************/
#ifndef PYTHONPATHS_H
    #include "pythonpaths.h"
#endif // PYTHONPATHS_H
//--------------------------------
#include <QCoreApplication>
#include <QFileInfo>
/*****************************************************************************/
// to simplify making the paths for the script
//++++++++++++++++++++++++++++++++
jfPythonPaths::jfPythonPaths(const QString invoke_command, const QString script_filename,
                             const QString transfer_filename, const QDir* base_path) {
    is_valid = false;
    dirSet = false;
    invoke = invoke_command.trimmed();
    fname_script = script_filename.trimmed();
    fname_transfer = transfer_filename.trimmed();
    if (!CheckNonEmpty()) return;
    // diectory / path resolving
    if (base_path == NULL) {
        basepath = new QDir(QCoreApplication::applicationDirPath());
    }
    else { basepath = base_path; }
    script_path = MakeFullPath(fname_script, basepath);
    if (script_path.isEmpty()) return;
    transfer_path = MakeFullPath(fname_transfer, base_path);
    if (transfer_path.isEmpty()) return;
    // finishing
    dirSet = (base_path != NULL);
    is_valid = true;
}
//++++++++++++++++++++++++++++++++
// output
bool jfPythonPaths::IsValid() const { return is_valid; }
bool jfPythonPaths::DirectorySet() const { return dirSet; }
QString jfPythonPaths::InvokeCmd(bool checkquote) const {
    if (checkquote && invoke.contains(' ')) { return "\"" + invoke + "\""; }
    else return invoke;
}
QString jfPythonPaths::ScriptPath(bool checkquote) const {
    if (checkquote && script_path.contains(' ')) { return "\"" + script_path + "\""; }
    return script_path;
}
QString jfPythonPaths::TransferPath(bool checkquote) const {
    if (checkquote && transfer_path.contains(' ')) { return "\"" + transfer_path + "\""; }
    return transfer_path;
}
//---------------
bool jfPythonPaths::ScriptExists() const {
    QFileInfo scriptPath = QFileInfo(script_path);
    return scriptPath.exists() && scriptPath.isFile();
}
//++++++++++++++++++++++++++++++++
bool jfPythonPaths::SetTransferName(const QString& new_name) {
    if (new_name.isEmpty()) return false;
    QString npath = MakeFullPath(new_name, basepath);
    if (npath.isEmpty()) return false;
    fname_transfer = new_name;
    transfer_path = npath;
    is_valid = CheckNonEmpty() && (!script_path.isEmpty());
    return true;
}
//++++++++++++++++++++++++++++++++
jfPythonPaths* jfPythonPaths::CopyWithNewTransferName(const QString& new_name) const {
    const QDir* outdir = (dirSet) ? basepath : NULL;
    return new jfPythonPaths(invoke, fname_script, new_name, outdir);
}
//-------------
jfPythonPaths::~jfPythonPaths() {
    if ((dirSet == false) && (basepath != NULL)) {
        delete basepath;
        basepath = NULL;
    }
}
//++++++++++++++++++++++++++++++++
bool jfPythonPaths::CheckNonEmpty() const {
    if (invoke.isEmpty() || fname_script.isEmpty()) return false;
    return (!fname_transfer.isEmpty());
}
QString jfPythonPaths::MakeFullPath(QString start, const QDir* base_folder) const {
    if (start.isEmpty()) return "";
    QFileInfo startPath = QFileInfo(start);
    if (startPath.isRelative()) {
        startPath = QFileInfo(*base_folder,start);
        startPath.makeAbsolute();
    }
    return QDir::cleanPath(startPath.absoluteFilePath());
}
/*****************************************************************************/
