/******************************************************************************
Name    :   ao3_specials3.h
Author  :   John Q Metro
Purpose :   AO3 fandom and kudos filters
Created :   May 14, 2014
Updated :   May 14, 2014
******************************************************************************/
#ifndef AO3_SPECIALS3_H
#define AO3_SPECIALS3_H
#endif // AO3_SPECIALS3_H
//--------------------------------------
#ifndef JFBASEFILTER
  #include "../base/filterbase.h"
#endif
#ifndef SEXP_FILTERS_H_INCLUDED
  #include "../base/sexp_filters.h"
#endif // SEXP_FILTERS_H_INCLUDED
#ifndef MINMAX_FILTER_H
  #include "../extended/minmax.h"
#endif // MINMAX_FILTER_H
/*****************************************************************************/
/* Intended replacement for jfACategoriesFilter, with crossover boolean, forall
filter (mainly intended to allow excluding intra-universe crossovers) and the
usual expression filter operating on the names of all fandoms */
class jfAO3FandomFilter : public jfBaseFilter {
  public:
    QString parse_error;
    // constructors
    jfAO3FandomFilter();
    jfAO3FandomFilter(const jfAO3FandomFilter& source);
    // checks if the filter has no effect
    virtual bool isEmpty() const;
    // loading from a string representation
    virtual bool FromString(const QString& sourcedata);
    virtual QString ToString() const;
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
    virtual QString GetTypeDescription() const;
    virtual jfBaseFilter* GenCopy() const;
    virtual QString GetTypeID() const;
    // destructor
    ~jfAO3FandomFilter();
  protected:
    // the core matching method
    virtual bool CoreMatch(const jfSearchResultItem* testelem) const;
    // file i/o
    virtual bool AddRestToFile(QTextStream* outfile) const;
    virtual bool ReadRestFromFile(jfFileReader* infile);
    /* since different types are stored together, the text file reprentation
    may have objects of varying length */
    virtual size_t ExtraLines() const;
    // extra methods
    jfElemArray* ParseInput(QString srcstring,QString oerr);
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
// numeric filter that works via the number of kudos
class jfAO3KudoFilter : public jfMinMaxUFilter {
  public:
    // constructors
    jfAO3KudoFilter();
    jfAO3KudoFilter(size_t inmin, size_t inmax);
    jfAO3KudoFilter(const jfAO3KudoFilter& source);
    // redefined virtual methods
    virtual QString GetTypeID() const override;
    virtual QString GetTypeDescription() const override;
    virtual jfBaseFilter* GenCopy() const override;
  protected:
    // the core matching method
    virtual bool CoreMatch(const jfSearchResultItem* testelem) const override;
};
/*****************************************************************************/
