/*****************************************************************************
Name    : ffncatparser2.h
Basic   : Fanfiction.Net category parsing, crossover versions
Author  : John Q Metro
Started : July 20, 2016
Updated : October 22, 2018

******************************************************************************/
#ifndef FFNCATPARSER2_H
  #define FFNCATPARSER2_H
#endif // FFNCATPARSER2_H
//-----------------------------------
#ifndef FFNCATPARSER1_H
  #include "ffncatparser1.h"
#endif // FFNCATPARSER1_H
//---------------------------
#ifndef CAT_COLLECTIONS_H
  #include "../categories/cat_collections.h"
#endif // CAT_COLLECTIONS_H
#ifndef CROSSOVER_GROUPS_H
  #include "../categories/crossover_groups.h"
#endif // CROSSOVER_GROUPS_H
/******************************************************************************/
/* This parser is supposed to parse the crossover category listings for a
section. It produces results of type jfFFN_CrossoverSection  */
class jfFFNCrossoverSectionParser : public jfFFNCatParserBase {
  public:
    // constructor
    jfFFNCrossoverSectionParser();
    // custom methods
    virtual bool ParsePageCore(size_t pageindex);
    virtual void* getResults();
  protected:
    jfFFN_CrossoverSection* result_data;
};
//========================================================================
/* This parser is supposed to parse the crossover category listings for a
single fandom. It produces results of type jfFFN_CrossoverGroup  */
class jfFFNCrossoverParser : public jfFFNCatParserBase {
  public:
    // constructor
    jfFFNCrossoverParser();
    bool SetCrossoverStoragePointer(jfFFN_CrossCatStore* in_cross_ptr);
    bool SetHoldingSectionName(const QString& sname_in);
    // custom methods
    virtual bool ParsePageCore(size_t pageindex);
    virtual void* getResults();
  protected:
    jfFFN_CrossoverGroup* result_data;
    jfFFN_CrossCatStore* crossstore_ptr;
    QString sname;
};
/******************************************************************************/
