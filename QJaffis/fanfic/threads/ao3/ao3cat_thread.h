/******************************************************************************
Name    :   ao3cat_thread.h
Author  :   John Q Metro
Purpose :   Downloader class for AO3 sections (fandom listings)
Created :   July 15, 2016
Updated :   June 11, 2022
******************************************************************************/
#ifndef AO3CAT_THREAD_H
  #define AO3CAT_THREAD_H
#endif // AO3CAT_THREAD_H
//-----------------------------------
#ifndef GENCAT_THREAD_H
  #include "../gencat_thread.h"
#endif // GENCAT_THREAD_H
/*****************************************************************************/
// core downloader thread base class, managing multiple downloader threads
class jfAO3CategoryDownloader : public jfGeneralCategoryDownloaderBase {
    Q_OBJECT
  public:
    jfAO3CategoryDownloader();
    bool SetInitialData();
  protected:
    // virtual methods that have to be implemented by derived threads
    virtual jfPageParserBase* makeParser();
    virtual jfParseFetchPackage* MakeParserFetcher();

//    QString tlogname;
};
/*****************************************************************************/
