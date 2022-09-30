/*****************************************************************************
Name    : parsefetch_packagemaker.cpp
Basic   : Utility class for making instances of jfParseFetchPackage
Author  : John Q Metro
Started : May 8, 2022
Updated : July 3, 2022
******************************************************************************/
#ifndef PARSEFETCH_PACKAGEMAKER_H
    #include "parsefetch_packagemaker.h"
#endif // PARSEFETCH_PACKAGEMAKER_H
//-------------------------------
#ifndef FETCHQT_H
    #include "../fetching/core/fetchqt.h"
#endif // FETCHQT_H

#ifndef FETCHPYTHON_H
    #include "../fetching/core/fetchpython.h"
#endif // FETCHPYTHON_H

#ifndef INITEND_H_INCLUDED
  #include "../initend.h"
#endif // INITEND_H_INCLUDED

#include <QCoreApplication>
#include <QList>
/*****************************************************************************/
// constructor
jfPythonPathStore::jfPythonPathStore(const QString& paython_command_in, const QString& working_subfolder) {
    python_command = paython_command_in;
    QString base_path_string = QCoreApplication::applicationDirPath();
    if (working_subfolder.length() > 0) base_path_string += "/" + working_subfolder;
    basePath = new QDir(base_path_string);
}
// -------
int jfPythonPathStore::PathsForTransferBase(const QString& transferBase) const {
    QString lc_transfer_base = transferBase.toLower();
    if (path_store.contains(lc_transfer_base)) {
        QVector<jfPythonPaths*>* pathList = path_store.value(lc_transfer_base, NULL);
        if (pathList == NULL) return 0;
        else return pathList->count();
    }
    else return 0;
}
// -------
jfPythonPaths* jfPythonPathStore::makePathsFor(const jglobal::jfFetchBasics &fetch_info) {
    if (fetch_info.IsValid()) {
        QString lc_transfer_base = fetch_info.TransferBase().toLower();
        QVector<jfPythonPaths*>* pathList = path_store.value(lc_transfer_base);
        if (pathList == NULL) {
            pathList = new QVector<jfPythonPaths*>();
            path_store.insert(lc_transfer_base, pathList);
        }
        // we have a list of path objects
        int amount = pathList->count();
        QString transfer_filename = fetch_info.TransferBase() + QString::number(amount+1) + ".result";
        jfPythonPaths* new_path = new jfPythonPaths(python_command, fetch_info.ScriptFilename(), transfer_filename, basePath);
        pathList->append(new_path);
        // done
        return new_path;
    }
    else return NULL;
}
//-------------
bool jfPythonPathStore::newPythonCommand(const QString& python_command_in) {
    if (python_command_in.isEmpty()) return false;
    else {
        python_command = python_command_in;
        return true;
    }
}
// -------
jfPythonPathStore::~jfPythonPathStore() {
    QVector<jfPythonPaths*>* pathList = NULL;
    // delete the python paths
    QList<QString> transferKeys = path_store.keys();
    for (int i = 0; i < transferKeys.size(); i++) {
        pathList = path_store.take(transferKeys.at(i));
        if (pathList != NULL) {
            while(pathList->isEmpty() == false) {
                delete pathList->takeLast();
            }
            delete pathList;
        }
    }
    path_store.clear();
    delete basePath;
    basePath = NULL;
}

    // key is the transfer filename base
    // everytime a new makePathsFor is called (if valid), we make a new path with an extended filename
    // QMap<QString,QVector<jfPythonPaths*>*> path_store;

// ==============================================================================
jfParseFetchPackageMaker::jfParseFetchPackageMaker(const QString& python_command_in, const QString& working_subfolder) {
    pythonPaths = new jfPythonPathStore(python_command_in, working_subfolder);
    user_agents = new QStringList();
    user_agents->append("QJaffis");

}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfParseFetchPackage* jfParseFetchPackageMaker::makeGeneralFetchPackage(jfPageParserCore* parser, jfTestsDelegate* itester ) const {
    if ((parser == NULL) || (itester == NULL)) return NULL;
    jfPageFetchWithQt* qt_fetcher = new jfPageFetchWithQt(user_agents);
    jfParseFetchPackage* result = new jfParseFetchPackage(parser, itester, qt_fetcher, true);
    return result;
}
//--------------------------
jfParseFetchPackage* jfParseFetchPackageMaker::makePythonFetchPackage( jfPythonPaths* python_paths,
                                                jfPageParserCore* parser, jfTestsDelegate* itester ) const {
    if ((parser == NULL) || (itester == NULL) || (python_paths == NULL)) return NULL;
    jfPageFetchWithPython* python_fetcher = new jfPageFetchWithPython(python_paths);
    jfParseFetchPackage* result = new jfParseFetchPackage(parser, itester, python_fetcher, true);
    return result;
}
//--------------------------
jfParseFetchPackage*jfParseFetchPackageMaker::makeFetchPackage(const jglobal::jfFetchBasics &fetch_info,
                                                               jfPageParserCore* parser, jfTestsDelegate* itester) {
    if ((parser == NULL) || (itester == NULL)) return NULL;

    // invalid fetch basics means we use the default
    if ((fetch_info.IsValid() == false) || (fetch_info.FetchType() != jglobal::jfF_PYTHON)) {
        jfPageFetchWithQt* qt_fetcher = new jfPageFetchWithQt(user_agents);
        return new jfParseFetchPackage(parser, itester, qt_fetcher, true);
    }
    // python fetcher
    else {
        jfPythonPaths* custom_paths = pythonPaths->makePathsFor(fetch_info);
        jfPageFetchWithPython* python_fetcher = new jfPageFetchWithPython(custom_paths);
        return new jfParseFetchPackage(parser, itester, python_fetcher, true);
    }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// using old parser
jfParseFetchPackage* jfParseFetchPackageMaker::makeGeneralFetchPackage(jfPageParserBase* old_parser) const {
    if (old_parser == NULL) return NULL;
    jfPageFetchWithQt* qt_fetcher = new jfPageFetchWithQt(user_agents);
    jfParseFetchPackage* result = new jfParseFetchPackage(old_parser, qt_fetcher, true);
    return result;
}
// --------------------------------------------
jfParseFetchPackage* jfParseFetchPackageMaker::makePythonFetchPackage( jfPythonPaths* python_paths, jfPageParserBase* parser) const {
    if ((parser == NULL) || (python_paths == NULL)) return NULL;
    jfPageFetchWithPython* python_fetcher = new jfPageFetchWithPython(python_paths);
    jfParseFetchPackage* result = new jfParseFetchPackage(parser, python_fetcher, true);
    return result;
}
// --------------------------------------------
jfParseFetchPackage* jfParseFetchPackageMaker::makeFetchPackage(const jglobal::jfFetchBasics &fetch_info, jfPageParserBase* parser) const {
    if (parser == NULL) return NULL;

    // invalid fetch basics means we use the default
    if ((fetch_info.IsValid() == false) || (fetch_info.FetchType() != jglobal::jfF_PYTHON)) {
        jfPageFetchWithQt* qt_fetcher = new jfPageFetchWithQt(user_agents);
        return new jfParseFetchPackage(parser, qt_fetcher, true);
    }
    // python fetcher
    else {
        jfPythonPaths* custom_paths = pythonPaths->makePathsFor(fetch_info);
        jfPageFetchWithPython* python_fetcher = new jfPageFetchWithPython(custom_paths);
        return new jfParseFetchPackage(parser, python_fetcher, true);
    }
}
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++
jfPythonPaths* jfParseFetchPackageMaker::makePathsFor(const jglobal::jfFetchBasics &fetch_info) const {
    if (fetch_info.IsValid() && (fetch_info.FetchType() == jglobal::jfF_PYTHON)) {
        return pythonPaths->makePathsFor(fetch_info);
    }
    else return NULL;
}
// -----------------------------------------------------
jfPageFetchInterface* jfParseFetchPackageMaker::makeFetcherInterface(const jglobal::jfFetchBasics &fetch_info) const {
    if (fetch_info.IsValid() == false) return NULL;
    else if (fetch_info.FetchType() != jglobal::jfF_PYTHON) {
        return new jfPageFetchWithQt(user_agents);
    }
    // python fetcher
    else {
        jfPythonPaths* custom_paths = pythonPaths->makePathsFor(fetch_info);
        return new jfPageFetchWithPython(custom_paths);
    }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool jfParseFetchPackageMaker::newPythonCommand(const QString& python_command_in) {
    return pythonPaths->newPythonCommand(python_command_in);
}

// --------------------------------------------
jfParseFetchPackageMaker::~jfParseFetchPackageMaker() {
    delete pythonPaths;
    pythonPaths = NULL;
    delete user_agents;
    user_agents = NULL;
}

/*****************************************************************************/
