/******************************************************************************
Name    :   ffn_filteredit1.h
Author  :   John Q Metro
Purpose :   Declares some custom FNN Filter Editors
Created :   July 8, 2016 (classes taken from other files)
Updated :   July 8, 2016
******************************************************************************/
#ifndef FFN_FILTEREDIT1_H
  #define FFN_FILTEREDIT1_H
#endif // FFN_FILTEREDIT1_H
//-----------------------------------
#ifndef SFILT_EDIT_H_INCLUDED
  #include "../base/sfilt_edit.h"
#endif // SFILT_EDIT_H_INCLUDED
#ifndef FFN_SPECIAL1_H
  #include "../../../core/filters/ffn/ffn_special1.h"
#endif // FFN_SPECIAL1_H
#ifndef FILTERS_EDIT1_H_INCLUDED
  #include "../extended/filters_edit1.h"
#endif // FILTERS_EDIT1_H_INCLUDED
/*****************************************************************************/
/* a wrapper for jfSimpleExprEdit that is for a fanfiction.net category filter */
class jfFFN_CategoryFilterEditor : public jfSimpleSFilterEditor {
  public:
    // the default constructor
    jfFFN_CategoryFilterEditor(const jfFilterMap* infmap, const jfFFN_CategoryExprFilter* infilt, QWidget* parent = NULL);
  protected:
    // internal methods
    virtual jfSimpleExpFilterCore* MakeTypedFilter(jfSimpleExpr* inpval);
};
//================================================================================
// this is an editor for Fanfiction.Net genres
class jfFFNGenresFilterEditor : public jfTagFilterEditor {
  public:
    // the default constructor
    jfFFNGenresFilterEditor(const jfFilterMap* infmap, const jfFFNGenresFilter* infilt, QWidget* parent = NULL);
    // extra methods
    virtual jfTagFilterCore* GetTagFilter();
    virtual void LoadBlank();
    virtual bool isListLong() const;
};
//==========================================================================
class jfFFN_RatingEditPanel : public QWidget {
  public:
    // constructor
    jfFFN_RatingEditPanel(bool horizontal, const jfFFNRatingFilter* invalue, QWidget* parent = NULL);
    // setting and getting values
    bool StoreValue(jfFFNRatingFilter* saveto) const;
    bool LoadValue(const jfFFNRatingFilter* getfrom);
    jfFFNRatingFilter* GetValue() const;
  protected:
    // gui
    QCheckBox *kch, *kpch, *tch, *mch;
    // sizing and layout
    QHBoxLayout* bsizer;
    QGridLayout* fsizer;
    bool ishoriz;
};
//==========================================================================
// filter editor wrap for jfFFN_RatingEditPanel
class jfFFN_RatingFilterEditor : public jfBaseFilterEditor {
  public:
    // the default constructor
    jfFFN_RatingFilterEditor(const jfBaseFilter* infilt, const jfFilterMap* infmap, QWidget* parent = NULL);
    // implemented virtual methods
    virtual void LoadFilter(const jfBaseFilter* infilter);
    virtual jfBaseFilter* GetFilter();
    virtual bool GeneralCheck() const;
  protected:
    jfFFN_RatingEditPanel* insert_panel;
};
//===========================================================================
class jfFavsCountPanel : public jfMinMaxEditor {
  public:
    // constructor
    jfFavsCountPanel(bool wrapped, QWidget* parent = NULL );
  protected:
};
//-------------------------------------------
// A Fanfiction.Net Favs filter editor, a wrapper for jfMinMaxUPanel
class jfFFNFavsFilterEditor : public jfBaseFilterEditor {
  public:
    // the default constructor
    jfFFNFavsFilterEditor(const jfFilterMap* infmap, const jfFFNFavsFilter* infilt, QWidget* parent = NULL);
    // implemented virtual methods
    virtual void LoadFilter(const jfBaseFilter* infilter);
    virtual jfBaseFilter* GetFilter();
    // checking
    bool GeneralCheck() const;
  private:
    jfFavsCountPanel* insert_panel;
};
/*****************************************************************************/
