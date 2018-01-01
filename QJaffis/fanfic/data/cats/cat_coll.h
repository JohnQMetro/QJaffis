/******************************************************************************
Name    :   cat_coll.h
Author  :   John Q Metro
Purpose :   General classes and methods for holding site category info (multiple
            section, indexing, etc).
Created :   August 11, 2012
Conversion to Qt Started September 28, 2013
Updated :   September 10, 2016
******************************************************************************/
#ifndef CAT_COLL_H_INCLUDED
#define CAT_COLL_H_INCLUDED
#endif // CAT_COLL_H_INCLUDED
//------------------------------------------------------------
#ifndef GEN_CAT_H_INCLUDED
  #include "gen_cat.h"
#endif // GEN_CAT_H_INCLUDED
#ifndef SEXP_FILTERS_H_INCLUDED
  #include "../../../core/filters/base/sexp_filters.h"
#endif // SEXP_FILTERS_H_INCLUDED
//-------------------------------------
#include <QMutex>
//*****************************************************************************
// similar to jfFFN_CatP_SelCat, but greatly simplified due to the lack of crossover cats
class jfGenCat_PickData : public jfBaseObj {
  public:
    // the constructor
    jfGenCat_PickData();
    // index methods
    void ResetIndex();
    bool NextIndex();
    bool BackIndex();
    bool ToIndex(size_t toindex);
    int WhichIndex() const;
    size_t categoryCount() const;
    // info at the current index
    QString GetSection() const;
    QString GetCatName() const;
    // extra methods
    QStringList* GetListing() const;
    // setting the expression filters
    bool SetAndFilter(jfSimpleExpr* in_value);
    bool SetOrFilter(jfSimpleExpr* in_value);
    // getting the filter strings
    QString GetAnd() const;
    QString GetOr() const;
    // evaluating the filters
    bool EvalAndExpr(QString insrc) const;
    bool EvalOrExpr(QString insrc) const;
    bool NotEmptyFil() const;
    // adding data
    void AddSection(const QString& secname, bool duplicheck);
    void AddCategory(const QString& secname, const QString& catname, bool duplicheck);
    // removing data
    bool RemoveSection(const QString& secname);
    bool RemoveCategory(const QString& secname, const QString& catname);
    bool RemoveByString(const QString& inval);
    // finding data
    QStringList* FindSection(const QString& inname, QString top="") const;
    // implemented virtual methods
    virtual QString GetTypeID() const;
    // destructor
    ~jfGenCat_PickData();
  protected:
    // private helper methods
    bool RemoveAtIndex(size_t inindex);
    bool FindItem(size_t& outloc, QString first, QString second);
    QString Item2Str(size_t which) const;
    // private implemented virtual methods
    virtual bool AddMiddleToFile(QTextStream* outfile) const;
    virtual bool AddRestToFile(QTextStream* outfile) const;
    virtual bool ReadMiddleFromFile(jfFileReader* infile);
    virtual bool ReadRestFromFile(jfFileReader* infile);
    /* holding the names: 1st section, second the category */
    QStringList sec_name;
    QStringList cat_name;
    // the index
    int item_index;
    size_t item_count;
    // the expressions
    jfSimpleExpr* and_filter;
    jfSimpleExpr* or_filter;
};
//========================================================================
// forward declaration
jfGenSection* MakeTypedSection(const QString& intype );
//=========================================================================
// this big class is for holding all the category collections
class jfGen_CatHolder : public jfBaseObj {
  public:
    // the constructor
    jfGen_CatHolder();
    // Index Manipulation
    void ResetIndex(const size_t& which);
    bool NextIndex(const size_t& which);
    int GetSindex(const size_t& which) const;
    int GetCindex(const size_t& which) const;
    // category getting
    const jfGeneralCategory* CatAtIndex(const size_t& which) const;
    const jfGeneralCategory* GetNextCat(const size_t& which);
    const jfGeneralCategory* FindCatFromFinder(const QString& infind) const;
    const jfGeneralCategory* FindByUrl(const QString& infind) const;
    // more category getting
    const jfGeneralCategory* FindCatByName(const QString& section, const QString& catname) const;
    const jfGeneralCategory* FindCatByID(const QString& section, const size_t& id_to_find) const;
    // setting info
    void ClearOld();
    bool AddSection(jfGenSection* insection);
    // setting the marks
    bool ClearMarks(size_t which);
    size_t ApplyCatPicker(jfGenCat_PickData* catpicker, size_t which);
    // other special methods
    virtual QString GetTypeID() const;
    QStringList* GetCatNames(size_t sindex) const;
    QStringList* GetSecNames() const;
    size_t SCount() const;
    // the destructor
    ~jfGen_CatHolder();
  protected:
    // extra methods
    void ResetAllIndexes();
    bool NextIndexNoSkip(const size_t& which);
    int FindSection(const QString& snamein) const;
    // file i/o output
    virtual bool AddMiddleToFile(QTextStream* outfile) const;
    virtual bool AddRestToFile(QTextStream* outfile) const;
    virtual bool ReadMiddleFromFile(jfFileReader* infile);
    virtual bool ReadRestFromFile(jfFileReader* infile);
    // internal data
    mutable QMutex manip_mutex;
    mutable jfGeneralCategory* fbu_cache;
    // one set for all the sections
    std::vector<jfGenSection*> sections;
    size_t sec_count;
    // indexing
    int sec_idxs[32];
    int cat_idxs[32];
};
//*****************************************************************************
