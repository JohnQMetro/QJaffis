/******************************************************************************
Name    :   ficbase.h
Author  :   John Q Metro
Purpose :   Declares fanfic objects that hold fanfic info
Created :   April 2, 2012
// Conversion to QT started : April 20, 2013
Updated :   August 9, 2022
// Renamed from ficobj2.h
******************************************************************************/
#ifndef FICBASE_H
  #define FICBASE_H
#endif // FICBASE_H
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#ifndef BASEOBJ_H_INCLUDED
    #include "../../core/objs/baseobj.h"
#endif // BASEOBJ_H_INCLUDED
#ifndef JFMISCTYPES1
  #include "../../core/objs/misc_types1.h"
#endif
#ifndef FICEXTRACT_H_INCLUDED
  #include "../../ficdown/data/ficextract.h"
#endif // FICEXTRACT_H_INCLUDED

#ifndef DISPLAYHTMLSPEC_H
    #include "displayhtmlspec.h"
#endif // DISPLAYHTMLSPEC_
//****************************************************************************
class jfGenericFanfic : public jfBasePD {
  public:
    // default constructor
    jfGenericFanfic();
    jfGenericFanfic(const jfGenericFanfic& src);
    // getting values
    QString GetAuthor() const;
    QString GetAuthorUrl() const;
    size_t GetChapterCount() const;
    QDate GetUpdates() const;
    QString GetPDesc(size_t elen) const;
    // getting the extract
    jfFicExtract* GetExtract() const;
    // data setting methods
    void SetMissing();
    void SetNew(bool ival);
    bool ChangeAuthorname(QString newname);
    // special url construction
    // virtual QString GetURLforPart(size_t partindex) const = 0;
    virtual QString MakeAuthorUrl() const = 0;

  protected:
    // extra methods
    void LoadMoreValues1(jfSkeletonParser* inparser) const;
    void StoreToCopy(jfGenericFanfic* destination) const;
    QString MakeUStatus() const;
    QString DisplayHTMLHeader(size_t mindex) const;
    QString DisplayHTMLHeader(size_t mindex, const jfDisplayHTMLHelper* helper ) const;
    // special methods
    virtual jfFicExtract* MakeExtract() const = 0;
    virtual void LoadIntoExtract(jfFicExtract* into) const = 0;
    // core extract methods
    void LoadIntoExtractCore(jfFicExtract* into) const;
    // fic mark methods
    bool IsMarked(size_t mindex) const;
    QString GetMarkColor(size_t mindex) const;
    // internal data
    QString author_name;
    size_t part_count;
    jf_FUpdateStatus ustatus;
    QString author_url;
    QDate updated_date;
};
//==================================================================
class jfGenericFanfic2 : public jfGenericFanfic {
  public:
    // default constructor
    jfGenericFanfic2();
    jfGenericFanfic2(const jfGenericFanfic2& src);
    // getting values
    size_t GetWordcount() const;
    bool IsComplete() const;
  protected:
    // extra methods
    void LoadMoreValues2(jfSkeletonParser* inparser) const;
    // internal data
    size_t word_count;
    bool completed;
};

//==================================================================
class jfGenericFanfic3 : public jfGenericFanfic2 {
  public:
    // default constructors
    jfGenericFanfic3();
    jfGenericFanfic3(const jfGenericFanfic3& src);
    // data getting methods
    QString GetGenres() const;
    size_t GetAuthorID() const;
    void ChangeAuthorID(size_t newid);
  protected:
    // file i/o output
    virtual bool AddRestToFile(QTextStream* outfile) const;
    virtual bool ReadRestFromFile(jfFileReader* infile);
    virtual bool AddExtraStuff(QTextStream* outfile) const = 0;
    virtual bool ReadExtraStuff(jfFileReader* infile) = 0;
    // more data
    size_t author_id;
    QString genres;
};
//****************************************************************************
