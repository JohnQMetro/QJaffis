/***************************************************************
 * Name:      ficupdate_thread.h
 * Purpose:   Downloaded fanfic updater thread
 * Author:    John Q Metro
 * Created:   July 25, 2016
 * Updated:   July 26, 2016
 *
 **************************************************************/
#ifndef FICUPDATE_THREAD_H
  #define FICUPDATE_THREAD_H
#endif // FICUPDATE_THREAD_H
//------------------------------
#ifndef FICEXTRACT_H_INCLUDED
  #include "../data/ficextract.h"
#endif // FICEXTRACT_H_INCLUDED
#ifndef FICDOWN_TBASE_H
  #include "ficdown_tbase.h"
#endif // FICDOWN_TBASE_H
//-------------------------------------
#include <vector>
/**************************************************************/
// fic extracts
class jfFicExtractArray : public std::vector<jfFicExtract*> {
  public:
    // returns true if the id is within an extract in the array
    bool IdIncluded(const size_t& idtocheck,jf_FICTYPE idcheck) const;
    // returns a pointer to an extract by id
    jfFicExtract* GetByID(const size_t& idtocheck,jf_FICTYPE idcheck) const;
    // handles file deletion
    size_t DeleteSourceFiles(size_t inindex);
    bool DeleteOnlySource(size_t inindex);
    bool ClearSourceFiles(size_t inindex);
    // deletes all the contained fics and empties the array
    void ClearContents();
    // returns the fictype
    jf_FICTYPE GetFtype(size_t index) const;
};
//*************************************************************/
// update codes
namespace ficur {
  enum jfFICUR {NotUpdated,Updated,Missing,ParseError,Failed,Halted};
}
Q_DECLARE_METATYPE(ficur::jfFICUR);
//*************************************************************/
class jfFicUpdateThread : public jfFicDownloaderBase {
    Q_OBJECT
  public:
    jfFicUpdateThread(size_t in_max_threads);
    bool SetParams(const QString& indir, const size_t& in_sizeguide, bool in_docomplete);
  signals:
    void sendOneActionSection(QString action,QString what);
    void sendSectionCount(size_t sec_count);
    void sendNewSection(struct jfItemInfoMsg catdat);
    void sendSectionDone(bool skip);
    void sendSectionFail();
    void sendUpdateResult(ficur::jfFICUR ustatus, QString titleauth);
    void sendNothing(bool fics);
    void sendFicCount(size_t ficcount);
  public slots:
    virtual void StartProcessing();
  protected:
    bool InterCatCheckStop();
    // component parts (pre fic-download)
    size_t LoadHtmlFilelist();
    size_t CheckAllFiles();
    bool CheckHtmlFile(size_t index);
    jfFicExtract* MakeExtract(jf_FICTYPE& outtype, QString ficcore);
    bool DoAllFanfics();
    bool HandleFanficResult(bool fres);

    // custom virtual abstract methods
    virtual bool TestForAbort(jfFicExtract* new_extract) const;
    virtual bool SetFileBase();

    jfItemInfoMsg section_msg_data;
    size_t phase;
    size_t fic_index;
    bool halting;
    bool docomplete;
    size_t sizeguide;

    //loading info
    QString sourcedir;
    QStringList* filelist;
    QStringList* subdirs;
    jfFicExtractArray* ficlist;

};


/**************************************************************/
