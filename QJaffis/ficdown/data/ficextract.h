/******************************************************************************
Name    :   ficextract.h
Author  :   John Q Metro
Purpose :   Declares Basic types for downloading fanfics
Created :   March 15, 2012
Conversion to QT started : April 18, 2013
Updated :   July 2, 2022 (harrypotterfanfiction is dead)
******************************************************************************/
#ifndef FICEXTRACT_H_INCLUDED
  #define FICEXTRACT_H_INCLUDED
#endif // FICEXTRACT_H_INCLUDED
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#ifndef BASEOBJ_H_INCLUDED
    #include "../../core/objs/baseobj.h"
#endif // BASEOBJ_H_INCLUDED
#ifndef VECTORS_H_INCLUDED
  #include "../../core/structs/vectors.h"
#endif // VECTORS_H_INCLUDED
#include <QColor>
//****************************************************************************
const QString STARTINFO_HEADER = "$##@ FANFIC STARTINFO $##@";
//===========================================================================
/* jfft_FFN is Fanfiction.Net, jfft_AO3 is An Archive of Our Own, jfft_MMO is Mediaminer.org,
jfft_FIM is Frienship is Magic fiction */
enum jf_FICTYPE { jfft_FFN = 0, jfft_AO3 = 1, jfft_MMO = 2, jfft_FIM = 3};
//===========================================================================
// fictype into string and vice versa
QString Fictype2String(jf_FICTYPE intype);
bool String2FicType(const QString& inval, jf_FICTYPE& resval);
bool SiteHasIndexpage(const jf_FICTYPE intype);
bool IsValidFictype(const jf_FICTYPE& intype);
//========================================================================
// step 1 in string to fic extract conversion
jfLineParse* ParseExtractSource(const QString& inval, jf_FICTYPE& resval);
//===========================================================================
// holds data necessary for download and display
class jfFicExtract {
  public:
    // values
    size_t  fic_id;
    size_t  pcount;
    QString fic_title;
    QString author_name;
    size_t  auth_id;
    QDate   updated_date;
    bool    complete;

    QString fname_core;
    // only used in updating
    QStringList* fnamelist;

    QStringList partnames;
    // constructors
    jfFicExtract();
    jfFicExtract(const jfFicExtract& source);
    virtual ~jfFicExtract();
    // virtual methods
    virtual QString MakeUrl(size_t inval) const = 0;
    virtual QString FirstUrl() const = 0;
    virtual QString MakeAuthlink() const = 0;
    virtual jf_FICTYPE GetFicType() const = 0;
    // methods
    void LoadCoreValues(jfSkeletonParser* inparser) const;
    QString MakeTextForm() const;
    bool GetFromText(QString inval);
    bool GetFromParse(jfLineParse* lparser);
    QString ToLabelString();
    QString NameForPart(size_t index) const;
  private:
    // virtual methods to augment core methods
    virtual void LoadMoreValues(jfSkeletonParser* inparser) const = 0;
    virtual QString MoreTextForm() const = 0;
    virtual bool MoreFromText(jfLineParse* lparser_in) = 0;
};
//-----------------------------------
Q_DECLARE_METATYPE(jfFicExtract*);
//============================================================================
class jfFicExtract_FFN : public jfFicExtract {
  public:
    // constructors
    jfFicExtract_FFN();
    jfFicExtract_FFN(const jfFicExtract_FFN& source);
    // other methods
    virtual QString MakeUrl(size_t inval) const;
    virtual QString FirstUrl() const;
    virtual QString MakeAuthlink() const;
    virtual jf_FICTYPE GetFicType() const;
  private:
    // virtual methods to augment core methods
    virtual void LoadMoreValues(jfSkeletonParser* inparser) const;
    virtual QString MoreTextForm() const;
    virtual bool MoreFromText(jfLineParse* lparser_in);
};
//============================================================================
class jfFicExtract_FIM : public jfFicExtract {
  public:
    size_t wordcount;
    // constructors
    jfFicExtract_FIM();
    jfFicExtract_FIM(const jfFicExtract_FIM& source);
    // data methods
    virtual QString MakeUrl(size_t inval) const;
    virtual QString FirstUrl() const;
    virtual QString MakeAuthlink() const;
    virtual jf_FICTYPE GetFicType() const;
  private:
    // virtual methods to augment core methods
    virtual void LoadMoreValues(jfSkeletonParser* inparser) const;
    virtual QString MoreTextForm() const;
    virtual bool MoreFromText(jfLineParse* lparser_in);
};
//============================================================================
class jfFicExtract_cids : public jfFicExtract {
  public:
    jfIndexVector chapterids;
    // constructors
    jfFicExtract_cids();
    jfFicExtract_cids(const jfFicExtract_cids& source);
  private:
    // virtual methods to augment core methods
    virtual void LoadMoreValues(jfSkeletonParser* inparser) const;
    virtual QString MoreTextForm() const;
    virtual bool MoreFromText(jfLineParse* lparser_in);
    // more virtual methods to augment core methods
    virtual void LoadEvenMoreValues(jfSkeletonParser* inparser) const = 0;
    virtual QString EvenMoreTextForm() const = 0;
    virtual bool EvenMoreFromText(jfLineParse* lparser_in) = 0;
};

//===========================================================================
// mediaminer.org
class jfFicExtract_MMO : public jfFicExtract_cids {
  public:
    QString url_cname_part;
    // constructors
    jfFicExtract_MMO();
    jfFicExtract_MMO(const jfFicExtract_MMO& source);
    // virtual methods
    virtual QString MakeUrl(size_t inval) const;
    virtual QString FirstUrl() const;
    virtual QString MakeAuthlink() const;
    virtual jf_FICTYPE GetFicType() const;
  private:
    // more virtual methods to augment core methods
    virtual void LoadEvenMoreValues(jfSkeletonParser* inparser) const;
    virtual QString EvenMoreTextForm() const;
    virtual bool EvenMoreFromText(jfLineParse* lparser_in);
};
//===========================================================================
// mediaminoer.org
class jfFicExtract_AO3 : public jfFicExtract_cids {
  public:
    // constructors
    jfFicExtract_AO3();
    jfFicExtract_AO3(const jfFicExtract_AO3& source);
    // virtual methods
    virtual QString MakeUrl(size_t inval) const;
    virtual QString FirstUrl() const;
    virtual QString MakeAuthlink() const;
    virtual jf_FICTYPE GetFicType() const;
  private:
    // more virtual methods to augment core methods
    virtual void LoadEvenMoreValues(jfSkeletonParser* inparser) const;
    virtual QString EvenMoreTextForm() const;
    virtual bool EvenMoreFromText(jfLineParse* lparser_in);
};
//==========================================================================
jfFicExtract* CopyExtract(jfFicExtract* source);

//****************************************************************************
