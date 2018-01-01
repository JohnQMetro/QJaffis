/******************************************************************************
Name    :   fim2_filedit.h
Author  :   John Q Metro
Purpose :   More Editors for fim filters
Created :   August 5, 2015
Updated :   July 8, 2016 (moved thumbs up percent filter here)
******************************************************************************/
#ifndef FIM2_FILEDIT_H
#define FIM2_FILEDIT_H
#endif // FIM2_FILEDIT_H
//------------------------------------------------
#ifndef SFILT_EDIT_H_INCLUDED
  #include "../base/sfilt_edit.h"
#endif // SFILT_EDIT_H_INCLUDED
#ifndef WIDGETS3_H_INCLUDED
  #include "../../misc/widgets3.h"
#endif // WIDGETS3_H_INCLUDED
#ifndef FIMFILTERS1_H_INCLUDED
  #include "../../../core/filters/fim/fimfilters1.h"
#endif // FIMFILTERS1_H_INCLUDED
#ifndef FIMFILTERS2_H_INCLUDED
  #include "../../../core/filters/fim/fimfilters2.h"
#endif // FIMFILTERS2_H_INCLUDED
#ifndef MINMAX_EDITOR_H
  #include "../extended/minmax_editor.h"
#endif // MINMAX_EDITOR_H
/*****************************************************************************/

// a wrapper for jfSimpleExprEdit that is for the short FIM summary
class jfFIMShortDesc_FilterEditor : public jfSimpleSFilterEditor {
  public:
    // the default constructor
    jfFIMShortDesc_FilterEditor(const jfFilterMap* infmap, const jfFIM_ShortDesc_ExprFilter* infilt, QWidget* parent = NULL);
  protected:
    // internal methods
    virtual jfSimpleExpFilterCore* MakeTypedFilter(jfSimpleExpr* inpval);
};
//---------------------------------------
// a wrapper for jfSimpleExprEdit that is for the short FIM summary OR the regualr description
class jfFIMDualDesc_FilterEditor : public jfSimpleSFilterEditor {
  public:
    // the default constructor
    jfFIMDualDesc_FilterEditor(const jfFilterMap* infmap, const jfFIM_DualDesc_ExprFilter* infilt, QWidget* parent = NULL);
  protected:
    // internal methods
    virtual jfSimpleExpFilterCore* MakeTypedFilter(jfSimpleExpr* inpval);
};

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// the group story count filter editor, a specialization of jfZeroToMaxFilterEditor
class jfFIMGroupSCFilterEditor : public jfZeroToMaxFilterEditor {
  public:
    // the default constructor
    jfFIMGroupSCFilterEditor(const jfFilterMap* infmap, const jfFIMGroupSCFilter* infilt, QWidget* parent = NULL);
  private:
    virtual jfMinMaxUFilter* MakeTypedMinMax() const;
};
//--------------------------------------------------------------
// the group member count filter editor, a specialization of jfZeroToMaxFilterEditor
class jfFIMGroupMCFilterEditor : public jfZeroToMaxFilterEditor {
  public:
    // the default constructor
    jfFIMGroupMCFilterEditor(const jfFilterMap* infmap, const jfFIMGroupMCFilter* infilt, QWidget* parent = NULL);
  private:
    virtual jfMinMaxUFilter* MakeTypedMinMax() const;
};
//====================================================================================
// filter editor for jfFimThumbPercentFilter
class jfFimThumbPercentFilterEditor : public jfBaseFilterEditor {
    // the default constructor
  public:
    jfFimThumbPercentFilterEditor(const jfFilterMap* infmap, const jfFimThumbPercentFilter* infilt, QWidget* parent = NULL);
    // implemented virtual methods
    virtual void LoadFilter(const jfBaseFilter* infilter);
    virtual jfBaseFilter* GetFilter();
    virtual bool GeneralCheck() const;
  protected:
    jfLabeledIntEdit* insert_panel;
};
/*****************************************************************************/
