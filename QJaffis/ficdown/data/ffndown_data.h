/////////////////////////////////////////////////////////////////////////////
// Name:        ffndown_data.h
// Author :     John Q Metro
// Purpose :    Downloading fics, we need structures to hold the data
// Created:     June 17, 2011
// Conversion to QT started : April 18, 2013
// Updated:     October 13, 2019
/////////////////////////////////////////////////////////////////////////////
#ifndef FFNDOWN_DATA_H_INCLUDED
#define FFNDOWN_DATA_H_INCLUDED
#endif // FFNDOWN_DATA_H_INCLUDED
//------------------------------------------
#ifndef SKELSTORE_H_INCLUDED
  #include "../../core/objs/skelstore.h"
#endif // SKELSTORE_H_INCLUDED
#ifndef FICEXTRACT_H_INCLUDED
  #include "ficextract.h"
#endif // FICEXTRACT_H_INCLUDED
#ifndef FICPART_H
  #include "ficpart.h"
#endif // FICPART_H
//---------------------------------------
#include <QString>
#include <QValidator>
#include <QRegExp>
//****************************************************************************
// a validator for filtering forbidden characters
class jfBadCharsFilter : public virtual QValidator {
public:
    jfBadCharsFilter(const QString badchars, QObject* parent = 0);
    QValidator::State validate(QString & input, int & pos) const;
    void fixup(QString & input) const;

protected:
  QRegExp cfilt;
};

//==========================================================================
// groups together skeletons and thier outputs
class jfFD_OutStuff {
  public:
    // public data
    jfSkeletonParser* parse;
    jfSkeletonFicPart* base;
    // constructor
    jfFD_OutStuff();
    // public methods
    bool CheckOk() const;
    QString MakeResult(QString inskel);
    // file i/o methods
    bool SetNameInfo(QString inname, QString indir, size_t num_files);
    bool GetFilename(QString& outresult, size_t gindex) const;
    bool StartNewFile();
    bool AddToFile(QString inval);
    bool CloseFile();
    // destructor
    ~jfFD_OutStuff();
  protected:
    // internal methods
    void ClearFiles();
    QString FormatNumber(size_t inval) const;
    // internal data
    std::vector<QTextStream*> outfiles;
    QStringList* filenames;
    bool fileopen;
    bool infoset;
    QString outname;
    QString outdir;
    size_t gcount;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// handles splitting the results into multiple files
class jfFanficSplitter {
  public:
    // constructor
    jfFanficSplitter(size_t sguide_in);
    // adding parts
    bool AddParts(QString inpart);
    // getting info
    size_t GetPartcount() const;
    size_t GetFilecount() const;
    size_t GetFileIndex(size_t file_idx, bool start) const;
    QString GetPart(size_t index) const;
    // analysis methods
    size_t CalculateSplit();
    // destructor
    ~jfFanficSplitter();
  protected:
    QStringList* prepared_parts;
    std::vector<float> prep_sizes;
    size_t size_guide;
    size_t ofile_count;
    std::vector<size_t> ofile_index;
};
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// holds a downloaded fanfic
class jfFanfic {
  public:
    // constructors
    jfFanfic(jf_FICTYPE intype);
    jfFanfic(jfFicExtract* inval);
    // data setting methods
    bool SetFic(jfFicExtract* inval, bool changefilebase=true);
    bool AddPart(jfFicPart* part_to_add);
    bool SetFileBase(QString inval, QString indir, size_t in_sguide);
    // info methods
    size_t PartCount() const;
    bool MultiParts() const;
    bool IsDataset() const;
    bool LoadCoreValues(jfSkeletonParser* inparser) const;
    // output methods
    bool BuildResult();
    bool StartResult();
    bool WriteFile(size_t findex_in);
    void FinishAfter();
    size_t GetFilecount() const;
    // destructors
    ~jfFanfic();
  protected:
    // internal methods
    void ClearStuff();
    QString PLabelMaker(size_t indexv) const;
    // main data
    jf_FICTYPE fictype;
    std::vector<jfFicPart*> maindata;
    jfFicExtract* sdata;
    bool dataset;
    QString filebase;
    QString dirbase;
    size_t sguide;
    // parsing stuff
    jfSkeletonFicPart* theskel;
    jfFanficSplitter* split_manager;
    jfFD_OutStuff* output_manager;
    bool output_started;

};
//****************************************************************************
