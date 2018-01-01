/******************************************************************************
// Name:        categories.h
// Author :     John Q Metro
// Purpose :    Declares collections involving categorization
// Created:     April 1, 2009
// Conversion to QT started : September 5, 2013
// Updated:     August 21, 2012
latest : Changing to use jfFileReader
******************************************************************************/
#ifndef CATEGORIES_H_INCLUDED
#define CATEGORIES_H_INCLUDED
#endif // CATEGORIES_H_INCLUDED
//-----------------------------------------------------------------------------

//---------------------------------------------------------------------------
#ifndef BASECOLLECT_H_INCLUDED
  #include "basecollect.h"
#endif // BASECOLLECT_H_INCLUDED
#ifndef EXPRESSION_H
  #include "../filters/base/expression.h"
#endif // EXPRESSION_H
//****************************************************************************
/* A Category is basically an expression filter that is used to sort the
results of a search (after filtering) into a particular group. Since a
category is pretty much an expression filter, we do not make a separate data
type, but just use jfExpressionFilter. We do use a custom collection class.

There is some extra per-category data for saving results though. We store
some filename/path info for placing the result files.
 */
//=========================================================================
class jfCategories : public jfBaseCollection {
  public:
    // data that we do not bother to hide
    jfFilterMap* localmap_ptr;
    QString base_outputdir;
    // the defualt constructor
    jfCategories();
    // adds categories
    jfExpressionFilter* AddNewCategory(const QString& newname="New Category");
    bool AppendCategory(jfExpressionFilter* replacement);
    // setting some information
    void SetUseDefault(bool tvalue);
    bool SetLocalMapPtr(jfFilterMap* invalue);
    bool ChangeDefaultFName(QString in_name);
    // a special method called when starting a search
    size_t CopyLoad();
    bool VerifyNames(QString& omsg);
    // getting info
    bool UsesDefault() const;
    bool NotEmpty() const;
    bool ChIndex(const size_t& catval) const;
    // uses the categories to mark the test element with catgeory info
    bool Sort(jfBasePD* testee);
    // old virtual methods we implement
    virtual bool NextIndex();
    virtual bool BackIndex();
    virtual void ResetIndex();
    virtual bool ToIndex(const int& newindex);
    virtual QString GetTypeID() const;
    // some more formerly abstract methods
    virtual QStringList* GetNameList() const;
    virtual QStringList* GetDescriptionList() const;
    virtual QStringList* GetTypeList() const;
    bool DeleteAtIndex(const size_t& dindex);
    // getting information
    jfExpressionFilter* GetAtIndex(size_t gindex);
    QString NameAtIndex(size_t gindex) const;
    QString DescAtIndex(size_t gindex) const;
    QString PathAtIndex(size_t gindex) const;
    bool RelativePathAtIndex(size_t gindex) const;
    QString GetFullPath(size_t gindex) const;
    // setting information
    void ReplaceAtIndex(size_t gindex, jfExpressionFilter* replacement);
    void SetFilePath(size_t gindex,const QString& src_fpath, bool relative);
    // the destructor
    ~jfCategories();
  protected:
    // implemented virtual methods
    virtual bool AddMid2ToFile(QTextStream* outfile) const;
    virtual bool AddRestToFile(QTextStream* outfile) const;
    virtual bool ReadMid2FromFile(jfFileReader* infile);
    virtual bool ReadRestFromFile(jfFileReader* infile);
    // internal data
    jfExpressionFilter* thecategories[32];
    // filename and path data
    QString* fname_path[33];
    bool frelative[33];
    // if we use the default category, we set this to true
    bool usedefault;

};


//***************************************************************************
