/////////////////////////////////////////////////////////////////////////////
// Name:        ffn_catpdata.h
// Author :     John Q Metro
// Purpose :    Holding data on which categories have been selected for searching
// Created:     May 10, 2010
// Conversion to Qt Started June 14, 2014
// Updated:     August 26, 2014
/////////////////////////////////////////////////////////////////////////////
#define FFNCATPDATA

#ifndef BASEOBJ_H_INCLUDED
    #include "../../core/objs/baseobj.h"
#endif // BASEOBJ_H_INCLUDED
#ifndef SEXP_FILTERS_H_INCLUDED
  #include "../../core/filters/base/sexp_filters.h"
#endif // SEXP_FILTERS_H_INCLUDED
#ifndef VECTORS_H_INCLUDED
  #include "../../core/structs/vectors.h"
#endif // VECTORS_H_INCLUDED
//**************************************************************************
// forward declaration
class jfFFN_Categories;
// holder for which categories have been specifically selected
class jfFFN_CatP_SelCat : public jfBaseObj {
  public:
    // the constructors
    jfFFN_CatP_SelCat();
    jfFFN_CatP_SelCat(jfFFN_Categories* incatdatlink);
    bool SetCatLink(jfFFN_Categories* incatdatlink);
    // index methods
    void ResetIndex();
    bool NextIndex();
    bool BackIndex();
    bool ToIndex(size_t toindex);
    int WhichIndex() const;
    // info at the current index
    bool IsCrossover() const;
    QString GetCrossSection() const;
    QString GetGroupSection() const;
    QString GetCatName() const;
    // extra methods
    QStringList* GetListing() const;
    // setting the expression filters
    bool SetAndFilter(jfSimpleExpr* in_value);
    bool SetOrFilter(jfSimpleExpr* in_value);
    // getting the filter strings
    bool FiltersNotEmpty() const;
    QString GetAnd() const;
    const jfSimpleExpr* GetAndFilter() const;
    QString GetOr() const;
    const jfSimpleExpr* GetOrFilter() const;
    // evaluating the filters
    bool NotEmptyFil() const;
    // adding data
    void AddSection(const QString& secname, bool duplicheck);
    void AddCategory(const QString& secname, const QString& catname, bool duplicheck);
    void AddCrossoverSection(const QString& secname, bool duplicheck);
    void AddCrossoverGroup(const QString& secname, const QString& groupname, bool duplicheck);
    void AddCrossoverCategory(const QString& secname, const QString& groupname, const QString& catname, bool duplicheck);
    // removing data
    bool RemoveSection(const QString& secname);
    bool RemoveCategory(const QString& secname, const QString& catname);
    bool RemoveCrossoverSection(const QString& secname);
    bool RemoveCrossoverGroup(const QString& secname, const QString& groupname);
    bool RemoveCrossoverCategory(const QString& secname, const QString& groupname, const QString& catname);
    bool RemoveByString(const QString& inval);
    // finding data
    QStringList* FindSection(const QString& inname) const;
    jfStrBoolList* FindSectionCross(const QString& sec1, const QString& cat1) const;
    QStringList* FindCrossSection(const QString& inname, bool incl_part) const;
    QStringList* FindCheckedCrossoverSections() const;
    // implemented virtual methods
    virtual QString GetTypeID() const;
    // destructor
    ~jfFFN_CatP_SelCat();
  protected:
    // private helper methods
    bool RemoveAtIndex(size_t inindex);
    void Append3(QString first, QString second, QString third);
    bool ThreeEq(int index, QString first, QString second, QString third);
    bool Equal3(int index, QString first, QString second, QString third);
    bool FindItem(size_t& outloc, QString first, QString second, QString third);
    QString Item2Str(size_t which) const;
    bool SplitCross(QString input, QString& sec_out, QString& cat_out) const;
    // private implemented virtual methods
    virtual bool AddMiddleToFile(QTextStream* outfile) const;
    virtual bool AddRestToFile(QTextStream* outfile) const;
    virtual bool ReadMiddleFromFile(jfFileReader* infile);
    virtual bool ReadRestFromFile(jfFileReader* infile);

    /* holding the names: 1st crossover section, 2nd section or crossover group,
    and third the category */
    QStringList cs_name;
    QStringList gs_name;
    QStringList cat_name;
    // the index
    int item_index;
    size_t item_count;
    // the expressions
    jfSimpleExpr* and_filter;
    jfSimpleExpr* or_filter;
    // external link
    jfFFN_Categories* catdatlink;
};
//**************************************************************************


