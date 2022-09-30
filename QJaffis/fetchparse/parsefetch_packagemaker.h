/*****************************************************************************
Name    : parsefetch_packagemaker.h
Basic   : Utility class for making instances of jfParseFetchPackage
Author  : John Q Metro
Started : May 8, 2022
Updated : July 3, 2022
******************************************************************************/
#ifndef PARSEFETCH_PACKAGEMAKER_H
    #define PARSEFETCH_PACKAGEMAKER_H
#endif // PARSEFETCH_PACKAGEMAKER_H
//-------------------------------
#ifndef DOWNLOADROOT_H
    #include "../fetching/loopget/downloadroot.h"
#endif // DOWNLOADROOT_H
#ifndef PYTHONPATHS_H
    #include "../fetching/core/pythonpaths.h"
#endif // PYTHONPATHS_H
#ifndef DEFAULTPATHS_H
    #include "../defaultpaths.h"
#endif // DEFAULTPATHS_H
// -------------------------------------------------

#include <QMap>
#include <QVector>
#include <QString>

/*****************************************************************************/

class jfPythonPathStore {
  public:
    // constructor
    jfPythonPathStore(const QString& paython_command_in, const QString& working_subfolder);
    int PathsForTransferBase(const QString& transferBase) const;
    jfPythonPaths* makePathsFor(const jglobal::jfFetchBasics &fetch_info);
    bool newPythonCommand(const QString& python_command_in);
    virtual ~jfPythonPathStore();
  protected:
    // key is the transfer filename base
    // everytime a new makePathsFor is called (if valid), we make a new path with an extended filename
    QMap<QString,QVector<jfPythonPaths*>*> path_store;
    QString python_command;
    QDir* basePath;
};
// ------------------------------------

class jfParseFetchPackageMaker {
  public:
    // constructor
    jfParseFetchPackageMaker(const QString& python_command_in, const QString& working_subfolder);

    // generic maker
    jfParseFetchPackage* makeGeneralFetchPackage(jfPageParserCore* parser, jfTestsDelegate* itester ) const;
    jfParseFetchPackage* makePythonFetchPackage( jfPythonPaths* python_paths, jfPageParserCore* parser, jfTestsDelegate* itester ) const;
    jfParseFetchPackage* makeFetchPackage(const jglobal::jfFetchBasics &fetch_info, jfPageParserCore* parser, jfTestsDelegate* itester);

    // using old parser
    jfParseFetchPackage* makeGeneralFetchPackage(jfPageParserBase* old_parser) const;
    jfParseFetchPackage* makePythonFetchPackage( jfPythonPaths* python_paths, jfPageParserBase* parser) const;
    jfParseFetchPackage* makeFetchPackage(const jglobal::jfFetchBasics &fetch_info, jfPageParserBase* parser) const;

    jfPythonPaths* makePathsFor(const jglobal::jfFetchBasics &fetch_info) const;
    jfPageFetchInterface* makeFetcherInterface(const jglobal::jfFetchBasics &fetch_info) const;

    bool newPythonCommand(const QString& python_command_in);
    virtual ~jfParseFetchPackageMaker();
  protected:
    jfPythonPathStore* pythonPaths;
    QStringList* user_agents;
};

/*****************************************************************************/
