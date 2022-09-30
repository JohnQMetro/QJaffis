/***************************************************************
 * Name:      ficdown_single.h
 * Purpose:   Fanfic downloader thread for one fanfic
 * Author:    John Q Metro
 * Created:   July 12, 2016
 * Updated:   July 12, 2016
 *
 **************************************************************/
#ifndef FICDOWN_SINGLE_H
  #define FICDOWN_SINGLE_H
#endif // FICDOWN_SINGLE_H
//------------------------------
#ifndef FICDOWN_TBASE_H
  #include "ficdown_tbase.h"
#endif // FICDOWN_TBASE_H
#ifndef FICEXTRACT_H_INCLUDED
  #include "../data/ficextract.h"
#endif // FICEXTRACT_H_INCLUDED
/**************************************************************/
// class to hold fic data passed to the downloader
class jfFicDownParameters {
  public:
    jfFicExtract* extract;
    jf_FICTYPE fictype;
    QString firsturl;
    size_t sizeguide;
    QString filename_base;
    QString write_to_directory;
  // constructors
    jfFicDownParameters();
    jfFicDownParameters(jfFicExtract* extract_in);
};

//==================================
class jfSingleFicDownloader : public jfFicDownloaderBase {
    Q_OBJECT
  public:
    jfSingleFicDownloader();
    bool SetFicParameters(jfFicDownParameters* in_ficparams);
  public slots:
    virtual void StartProcessing();
  protected:

    // custom virtual abstract methods
    virtual bool TestForAbort(jfFicExtract* new_extract) const;
    virtual bool SetFileBase();

    jfFicDownParameters* params;
};

/**************************************************************/
