/***************************************************************
 * Name:      ficdown_tbase.h
 * Purpose:   Base downloader thread for fanfic downloading (and writing)
 * Author:    John Q Metro
 * Created:   July 10, 2016
 * Updated:   July 10, 2016
 *
 **************************************************************/
#ifndef FICDOWN_TBASE_H
#define FICDOWN_TBASE_H
#endif // FICDOWN_TBASE_H
//-------------------------------
#ifndef DOWNLOAD_H
  #include "../../fetching/download.h"
#endif // DOWNLOAD_H
#ifndef FICEXTRACT_H_INCLUDED
  #include "../data/ficextract.h"
#endif // FICEXTRACT_H_INCLUDED
#ifndef FFNDOWN_DATA_H_INCLUDED
  #include "../data/ffndown_data.h"
#endif // FFNDOWN_DATA_H_INCLUDED
/**************************************************************/
class jfFicDownloaderBase : public jfBaseFirstPageDownloader {
    Q_OBJECT
  public:
    jfFicDownloaderBase(size_t in_max_threads);
  signals:
    void sendFicExtract(jfFicExtract* sent_extract);
  protected:
    // virtual methods that are implemented here
    virtual QString* makeURLforPage(size_t index) const;
    virtual bool ProcessResults(void* resultdata);
    virtual void DisposeResults(void* resultdata);
    virtual void PrepareItemInfo(size_t pageIndex);
    virtual jfPageParserBase* makeParser();
    // special first page
    virtual void makeFirstPageInfo();
    virtual void ProcessFirstPageResults(void* resultdata);
    // custom virtual abstract methods
    virtual bool TestForAbort(jfFicExtract* new_extract) const =0;
    virtual bool SetFileBase() = 0;
    // implemented fic download
    void SendFileMsg(const size_t& fileindex);
    void ClearData();
    bool DownloadFic();


    //internal data for doing fic downloads
    QString firsturl;
    jf_FICTYPE fictype;
    bool indexpage;
    jfFicExtract* fic_data;
    jfFanfic* fanfic;
    // fic wriring add_on
    size_t numfiles;
};
/**************************************************************/
