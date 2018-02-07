/******************************************************************************
Name    :   fim3_filedit.h
Author  :   John Q Metro
Purpose :   Editors for fim filters : 2018 and later
Created :   January 21, 2018
Updated :   January 21, 2018
******************************************************************************/
#ifndef FIM3_FILEDIT_H
    #define FIM3_FILEDIT_H
#endif // FIM3_FILEDIT_H
//-------------------------------
#ifndef FIMFILTERS3_H_INCLUDED
  #include "../../../core/filters/fim/fimfilters3.h"
#endif // FIMFILTERS3_H_INCLUDED
#ifndef FILTERS_EDIT1_H_INCLUDED
  #include "../extended/filters_edit1.h"
#endif // FILTERS_EDIT1_H_INCLUDED
/*****************************************************************************/
class jfFimContentTypeFilterEditor : public jfTagFilterEditor {
  public:
    // the default constructor
    jfFimContentTypeFilterEditor(const jfFilterMap* infmap, const jfFIMContentTypeFilter* infilt, QWidget* parent = NULL);
    // extra methods
    virtual jfTagFilterCore* GetTagFilter();
    virtual bool isListLong() const;
    virtual void LoadBlank();
};
//==========================================================
class jfFimWarningsFilterEditor : public jfTagFilterEditor {
  public:
    // the default constructor
    jfFimWarningsFilterEditor(const jfFilterMap* infmap, const jfFIMWarningsFilter* infilt, QWidget* parent = NULL);
    // extra methods
    virtual jfTagFilterCore* GetTagFilter();
    virtual bool isListLong() const;
    virtual void LoadBlank();
};
/*****************************************************************************/
