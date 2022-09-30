/***************************************************************
 * Name:      defaultpaths.h
 * Purpose:   A settings class that holds default directories
 * Author:    John Q Metro
 * Created:   December 30, 2021
 * Updated:   May 29, 2022
 **************************************************************/
#ifndef DEFAULTPATHS_H
    #define DEFAULTPATHS_H
#endif // DEFAULTPATHS_H
//--------------------------------
#ifndef UTILS2_H_INCLUDED
  #include "core/utils/utils2.h"
#endif // UTILS2_H_INCLUDED

#ifndef FICEXTRACT_H_INCLUDED
  #include "ficdown/data/ficextract.h"
#endif // FICEXTRACT_H_INCLUDED
//--------------------------------
#include <QStringList>
#include <QTextStream>
/*************************************************************/
namespace jglobal {

const size_t path_type_count = 11;

enum DEFAULT_PATH_TYPE {
    DEFAULT_PATH = 0,
    UPDATABLE_RESULTS = 1,
    SAVED_RESULTS = 2,
    SAVED_SEARCHES = 3,
    SAVED_FANFIC_A = 4,
    SAVED_FANFIC_B = 5,
    SAVED_FANFIC_C = 6,
    SYNC_SOURCE = 7,
    SYNC_TARGET = 8,
    PYTHON_SCRIPT_DEFAULT = 9,
    PYTHON_SCRIPT_WORK = 10
};

bool ValidPathType(DEFAULT_PATH_TYPE path_type);
bool IsDownloadPathType(DEFAULT_PATH_TYPE path_type);

// class holding all of the folders/directories for I/O
class jfDefaultPaths {
  public:
    jfDefaultPaths();
    // path info
    size_t TotalPathCount() const;
    bool HasPathFor(DEFAULT_PATH_TYPE path_type) const;
    bool HasValidPathFor(DEFAULT_PATH_TYPE path_type) const;
    QString GetPathFor(DEFAULT_PATH_TYPE path_type) const;
    QString LabelFor(DEFAULT_PATH_TYPE path_type) const;
    // changing paths
    void MakePathsDefault();
    bool MakePathDefaultFor(DEFAULT_PATH_TYPE path_type);
    bool SetPathFor(DEFAULT_PATH_TYPE path_type, const QString& changeto);
    bool MakeInvalidPathDefault();
    // file i/o
    bool LoadPathsFrom(jfFileReader* input_file);
    bool AddPathsTo(QTextStream* ofile) const;

  protected:
    bool IsValidPath(const QString& test_path) const;
    QString GetDefaultPath(bool downloads, const QString& fallback) const;
    QString DefaultDocPath() const;

    QStringList paths_holder;
    QStringList paths_labels;
};
// =========================================================
/* making the page fetch somewhat configurable */
enum jfFICPAGE_TYPE {
    FPT_GENERAL = 0,
    FPT_CATEGORY_PAGE = 1,
    FPT_LISTING_PAGE = 2,
    FPT_FICPART_PAGE = 3
};

enum jfFETCHER {
    jfF_NORMAL = 0,
    jfF_PYTHON = 1
};

// used to define how a page should be fetched
class jfFetchBasics {
  public:
    jfFetchBasics();
    // info
    bool IsValid() const;
    jf_FICTYPE SiteTag() const;
    jfFICPAGE_TYPE PageType() const;
    jfFETCHER FetchType() const;
    QString ScriptFilename() const;
    QString TransferBase() const;
    bool ForType(jf_FICTYPE site_tomatch, jfFICPAGE_TYPE page_type_tomatch) const;
    // I/O
    bool SetFromSCString(const QString& source);
    QString ToIOString() const;

  protected:
    bool is_valid;
    jf_FICTYPE site;
    jfFICPAGE_TYPE page_type;
    jfFETCHER fetch_type;
    QString script_filename;
    QString transfer_base;
};


}
/*************************************************************/
