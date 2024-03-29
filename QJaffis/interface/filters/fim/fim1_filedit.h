/******************************************************************************
Name    :   fim1_filedit.h
Author  :   John Q Metro
Purpose :   Editors for fim filters : thumbs rating
Created :   July 5, 2012
Conversion to Qt started October 15, 2013
Updated :   August 5, 2023
******************************************************************************/
#ifndef FIM1_FILEDIT_H_INCLUDED
#define FIM1_FILEDIT_H_INCLUDED
#endif // FIM1_FILEDIT_H_INCLUDED
//---------------------------------------------------------------------------
#ifndef FIMFILTERS1_H_INCLUDED
  #include "../../../core/filters/fim/fimfilters1.h"
#endif // FIMFILTERS1_H_INCLUDED
#ifndef WIDGETS3_H_INCLUDED
  #include "../../controls/widgets3.h"
#endif // WIDGETS3_H_INCLUDED
#ifndef BFILT_EDIT_H_INCLUDED
  #include "../base/bfilt_edit.h"
#endif // BFILT_EDIT_H_INCLUDED
#ifndef FILTERS_EDIT1_H_INCLUDED
  #include "../extended/filters_edit1.h"
#endif // FILTERS_EDIT1_H_INCLUDED
/*****************************************************************************/

// core of Thumbs Up / Thumbs Down filter editor
class jfFIMThumbsPanel : public QWidget {
  public:
    // the constructor
    jfFIMThumbsPanel(bool wrapped, QWidget* parent = NULL );
    // get
    jfFimThumbsFilter* GetNewFilter(const QString& startName) const;
    // set
    bool SetFromObj(const jfFimThumbsFilter* inval);
  protected:
    jfMinMaxEditor* minmax;
    QCheckBox *include_rdisabled;
    QBoxLayout *top_layout;

};
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// the actual Thumbs Up / Thumbs Down filter editor, a wrapper for jfFIMThumbsPanel
class jfFimThumbsFilterEditor : public jfBaseFilterEditor {
  public:
    // the default constructor
    jfFimThumbsFilterEditor(const jfFilterBase* infilt, QWidget* parent = NULL);
    // implemented virtual methods
    virtual void LoadFilter(const jfFilterBase* infilter) override;
    virtual jfFilterBase* GetFilter() override;
    // checking
    virtual bool GeneralCheck(const jfFilterMap* filter_group) const override;
  private:
    jfFIMThumbsPanel* insert_panel;
};
//============================================================================
class jfFimGenreFilterEditor : public jfTagFilterEditor {
  public:
    // the default constructor
    jfFimGenreFilterEditor(const jfFIMGenreFilter* infilt, QWidget* parent = NULL);
    // extra methods
    virtual jfTagFilterCore* GetTagFilter();
    virtual bool isListLong() const;
    virtual void LoadBlank();
};
//============================================================================
class jfFimCharacterFilterEditor : public jfTagFilterEditor {
  public:
    // the default constructor
    jfFimCharacterFilterEditor(const jfFIMCharacterFilter* infilt, QWidget* parent = NULL);
    // extra methods
    virtual jfTagFilterCore* GetTagFilter();
    virtual bool isListLong() const;
    virtual void LoadBlank();
};
//============================================================================
class jfFIM_RatingEditPanel : public QWidget {
  public:
    // constructor
    jfFIM_RatingEditPanel(bool horizontal, const jfFimRatingFilter* invalue, QWidget* parent = NULL);
    // setting and getting values
    bool StoreValue(jfFimRatingFilter* saveto) const;
    bool LoadValue(const jfFimRatingFilter* getfrom);
    jfFimRatingFilter* GetValue(const QString& name) const;
  protected:
    // gui
    QCheckBox *ech, *tch, *mch;
    QBoxLayout* msizer;
    bool ishoriz;
};
//==========================================================================
// filter editor wrap for jfFIM_RatingEditPanel
class jfFIM_RatingFilterEditor : public jfBaseFilterEditor {
  public:
    // the default constructor
    jfFIM_RatingFilterEditor(const jfFimRatingFilter* infilt, QWidget* parent = NULL);
    // implemented virtual methods
    virtual void LoadFilter(const jfFilterBase* infilter) override;
    virtual jfFilterBase* GetFilter() override;
    virtual bool GeneralCheck(const jfFilterMap* filter_group) const override;
  protected:
    jfFIM_RatingEditPanel* insert_panel;
};
/*****************************************************************************/
