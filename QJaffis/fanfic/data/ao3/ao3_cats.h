/******************************************************************************
Name    :   ao3_cats.h
Author  :   John Q Metro
Purpose :   Category classes for archiveofourown.org
Created :   August 20, 2012
Conversion to Qt Started September 28, 2013
Updated :   July 13, 2016
******************************************************************************/
#ifndef AO3_CATS_H_INCLUDED
#define AO3_CATS_H_INCLUDED
#endif // AO3_CATS_H_INCLUDED
//-------------------------------------------------------------------
#ifndef GEN_CAT_H_INCLUDED
  #include "../cats/gen_cat.h"
#endif // GEN_CAT_H_INCLUDED
#ifndef CAT_MISC_H_INCLUDED
  #include "../cats/cat_misc.h"
#endif // CAT_MISC_H_INCLUDED

/*****************************************************************************/
extern const QString AO3_STYPES;
//=====================================
class jfAO3_Category : public jfGeneralCategory {
  public:
    jfAO3_Category();
    virtual bool SetFromString(const QString& invalin, const QString& insection);
    virtual QString GetTypeID() const;
    size_t GetRCount() const;
    QString GetParseErr() const;
  protected:
    bool parseErr(const QString& in_err_msg);
    virtual bool AddRestToFile(QTextStream* outfile) const;
    virtual bool ReadRestFromFile(jfFileReader* infile);
    size_t rcount;
    QString parse_err;
};
//==============================================================================
class jfAO3Section : public jfGenSection {
  public:
    // constructors
    jfAO3Section();
    jfAO3Section(QString& inname);
    virtual QString GetTypeID() const;
  protected:
    // special methods
    virtual jfGeneralCategory* MakeTypedCategory() const;
};
//===========================================================================
namespace ao3_catdata {
  extern const QString aname;
  extern jfGenSecData* ao3_nameslinks;
  extern jfGenCatManager* ao3_catmanager;
  bool Initialize();
  void Closing();
}
/*****************************************************************************/
