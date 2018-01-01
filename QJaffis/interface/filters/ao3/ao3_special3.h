/******************************************************************************
Name    :   ao3_special3.h
Author  :   John Q Metro
Purpose :   Declares some interface panels for some AO3 Filters
Created :   May 14, 2014
Updated :   August 5, 2015
******************************************************************************/
#ifndef AO3_SPECIAL3_H
#define AO3_SPECIAL3_H
#endif // AO3_SPECIAL3_H
//------------------------------------
#ifndef BFILT_EDIT_H_INCLUDED
  #include "../base/bfilt_edit.h"
#endif // BFILT_EDIT_H_INCLUDED
#ifndef AO3_SPECIALS3_H
  #include "../../../core/filters/ao3/ao3_specials3.h"
#endif // AO3_SPECIALS3_H
#ifndef SFILT_EDIT_H_INCLUDED
  #include "../base/sfilt_edit.h"
#endif // SFILT_EDIT_H_INCLUDED
#ifndef WIDGETS3_H_INCLUDED
  #include "../../misc/widgets3.h"
#endif // WIDGETS3_H_INCLUDED
#ifndef MINMAX_EDITOR_H
  #include "../extended/minmax_editor.h"
#endif // MINMAX_EDITOR_H
//-----------------------------------
#include <QCheckBox>
/*****************************************************************************/
class jfAO3FandomFilterEditor : public jfBaseFilterEditor {
    Q_OBJECT
  public:
    // constructor
      jfAO3FandomFilterEditor(const jfAO3FandomFilter* infilt, const jfFilterMap* infmap, QWidget* parent = NULL);
    // implemented virtual methods
      virtual void LoadFilter(const jfBaseFilter* infilter);
      virtual jfBaseFilter* GetFilter();
    // checking
      bool GeneralCheck() const;
  signals:
      void SendCrossCheck(bool value);
  public slots:
      void HandleCrossoverCheck(int state);
  protected:
    // internal setup methods
      void MakeWidgets();
      void ArrangeSizers2();
    // GUI elements
      QCheckBox* crossover_box;
      QCheckBox* allmatch_box;
      QLabel* perf_label;
      jfSimpleExprEdit* perf_edit;
      QLabel* main_label;
      jfSimpleExprEdit* main_edit;
    // sizers
      QHBoxLayout* csizer;
      QVBoxLayout* msizer;
    // internal data
      const jfAO3FandomFilter* typed_fpointer;
};
//==============================================================================
// the actual kudo count filter editor, a specialization of jfZeroToMaxFilterEditor
class jfAO3KudoFilterEditor : public jfZeroToMaxFilterEditor {
  public:
    // the default constructor
    jfAO3KudoFilterEditor(const jfFilterMap* infmap, const jfAO3KudoFilter* infilt, QWidget* parent = NULL);
  private:
    virtual jfMinMaxUFilter* MakeTypedMinMax() const;
};
/*****************************************************************************/
