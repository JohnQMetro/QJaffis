/******************************************************************************
Name    :   ao3_specials3.h
Author  :   John Q Metro
Purpose :   AO3 fandom and kudos filters
Created :   May 14, 2014
Updated :   April 15, 2023
******************************************************************************/
#ifndef AO3_SPECIALS3_H
#define AO3_SPECIALS3_H
#endif // AO3_SPECIALS3_H
//--------------------------------------
#include "../base/basefilter.h"
#include "../base/sexp_filters.h"

#ifndef MINMAX_FILTER_H
  #include "../extended/minmax.h"
#endif // MINMAX_FILTER_H
/*****************************************************************************/
extern const jfFilterTypeMeta AO3_FANDOM_FILTER_INFO;
//================================================================
/* Intended replacement for jfACategoriesFilter, with crossover boolean, forall
filter (mainly intended to allow excluding intra-universe crossovers) and the
usual expression filter operating on the names of all fandoms */
class jfAO3FandomFilter : public jfFilterBase {
  public:
    // constructors
    jfAO3FandomFilter(const QString& filter_name);
    jfAO3FandomFilter(QString&& filter_name);
    jfAO3FandomFilter(const jfAO3FandomFilter& source);
    // checks if the filter has no effect
    virtual bool IsEmpty() const override;
    // loading from a string representation
    virtual QString ToString() const override;
    // specific data setting methods
    bool GeneralFromExpr(jfSimpleExpr* in_source);
    bool PerFandomFromExpr(jfSimpleExpr* in_source);
    void SetCrossover(bool iscross);
    void SetAllMatch(bool amatch);
    // getting data
    QString GetMainSrc() const;
    QString GetPerFandomSrc() const;
    bool GetCrossover() const;
    bool GetAllMatch() const;
    // Additional implemented methods
    virtual const jfFilterTypeMeta& GetTypeMetaInfo() const override;
    virtual jfFilterBase* GenCopy() const override;

    // destructor
    ~jfAO3FandomFilter();
  protected:
    virtual bool FromStringInner(const QString& sourcedata, QString& error_out) override;
    // the core matching method
    virtual bool CoreMatch(const jfSearchResultItem* testelem) const override;
    // file i/o
    virtual bool CompatReadRestFromFile(jfFileReader* infile) override;

    // extra methods
    jfElemArray* ParseInput(QString srcstring, QString& oerr) const;

    // internal data
    bool must_crossover;
    bool allmatch;
    QString perfmatch_src;
    jfElemArray* perfmatch_parsed;
    QString mainmatch_src;
    jfElemArray* mainmatch_parsed;
    bool loaded;
};
//==========================================================================
// numeric filter that works via the number of kudos or favs

extern const jfFilterTypeMeta FAVS_FILTER_INFO;
// ------------------------------------------------

class jfFavKudoFilter : public jfMinMaxUFilter {
  public:
    // constructors
    jfFavKudoFilter(const QString& name);
    jfFavKudoFilter(const QString& name,size_t inmin, size_t inmax);
    jfFavKudoFilter(const jfFavKudoFilter& source);
    // redefined virtual methods
    virtual const jfFilterTypeMeta& GetTypeMetaInfo() const override;
    virtual bool MatchTypeIdentifier(const QString to_match) const override;
    virtual jfFilterBase* GenCopy() const override;
  protected:
    // the core matching method
    virtual bool CoreMatch(const jfSearchResultItem* testelem) const override;
};
/*****************************************************************************/
