/*****************************************************************************
Name    : ffncatthread.h
Basic   : Fanfiction.Net category downloading
Author  : John Q Metro
Started : July 21, 2016
Updated : September 25, 2022

******************************************************************************/
#ifndef FFNCATTHREAD_H
  #define FFNCATTHREAD_H
#endif // FFNCATTHREAD_H
//------------------------------------
#ifndef DOWNLOADROOT_H
  #include "../../fetching/loopget/downloadroot.h"
#endif // DOWNLOADROOT_H
#ifndef CATGROUP_H
  #include "../categories/catgroup.h"
#endif // CATGROUP_H
#ifndef FFNCATPARSER2_H
  #include "ffncatparser2.h"
#endif // FFNCATPARSER2_H
/*****************************************************************************/
// core downloader thread base class, managing multiple downloader threads
class jfFFN_CategoryDownloader : public jfDownloadRoot {
    Q_OBJECT
  public:
    jfFFN_CategoryDownloader();
    bool SetCatManagerPtr(jfFFN_CatManager* in_catman_ptr, bool doupdate);
  signals:
    void SendSectionCount(size_t ccount);
    void SendNewSection(struct jfItemInfoMsg catdat);
    void SendSectionDone(bool skip);
    void SendSectionFail();
    // special signals for this thread only
    void SendSkippedPages(size_t skipcount);
    void SendOneThingSection(QString action, QString what);
  public slots:
    virtual void StartProcessing();
  protected:
    bool DoSections(bool iscrossover);
    bool DoCrossoverSection();
    bool UpdateMarking(); // updatemode only section
    // implemented virtual methods of the base downloader
    virtual QString* makeURLforPage(size_t index) const;
    virtual bool ProcessResults(void* resultdata);
    virtual void DisposeResults(void* resultdata);
    virtual void PrepareItemInfo(size_t pageIndex);
    virtual bool advanceFetchIndex();
    virtual bool advanceResultIndex();

    // parser
    jfPageParserBase* makeParser();
    virtual jfParseFetchPackage* MakeParserFetcher();
    jfFFNCrossoverParser* crossover_parser;

      //custom data
    jfFFN_CatManager* catman_ptr;
    jfFFN_Categories* category_data;
    size_t phase;
    size_t csection_index;
    jfItemInfoMsg sectiondata;
    bool updatemode;

};
/*****************************************************************************/
